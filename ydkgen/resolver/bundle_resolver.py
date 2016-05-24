import re
import os
import json
import logging
import tempfile
from git import Repo
from shutil import rmtree, copy
from collections import namedtuple, defaultdict
from ..common import iskeyword

logger = logging.getLogger('ydkgen')
logger.addHandler(logging.NullHandler())

VALID_URI = re.compile(r"(?P<url>.+)\?commit-id=(?P<id>.+)&path=(?P<path>.+)")
Local_URI = namedtuple('Local_URI', ['url'])
Remote_URI = namedtuple('RemoveURI', ['url', 'commitid', 'path'])
Repo_Dir_Pair = namedtuple('Repo_Dir_Pair', ['repo', 'dir'])
Version = namedtuple('Version', ['major', 'minor', 'patch'])

dd = lambda: defaultdict(dd)

def parse_uri(uri):
    """ Return Local_URI if uri is local else Remote_URI.

        For example:
            >>> remote_uri = 'http://repository?commit-id=commitid&path=path'.
            >>> parse_uri(remote_uri)
            RemoveURI(url='http://repository', commitid='commitid', path='path')
            >>> local_uri = 'file://relative/path/to/file'
            >>> parse_uri(local_uri)
            Local_URI(url='relative/path/to/file')

        Raises:
            YdkGenException if uri is malformed.
    """
    if uri.startswith('http'):
        p = VALID_URI.match(uri)
        if p:
            return Remote_URI(*p.groups())
        else:
            raise YdkGenException('Invalid file uri')

    elif uri.startswith('file'):
        return Local_URI(uri.split('file://')[-1])


class BundleDefinition(object):
    """ Base class for Bundle and BundleDependency, it has following attributes

        name (str): bundle name.
        _version (Version): bundle version.
        _ydk_version (Version): ydk core library used.

        For example:
            >>> d = {
            ...     'name' : 'name',
            ...     'version' : '0.3.0',
            ...     'ydk-version' : '0.4.0'
            ...     }
            >>> b = BundleDefinition(d)
            >>> b.fqn
            'ydk_name@0.3.0'

        Raises:
            KeyError if data is malformed.
    """

    def __init__(self, data):
        self._name = 'ydk_' + data['name']
        self._version = Version(*tuple(data['version'].split('.')))
        self._ydk_version = Version(*tuple(data['ydk-version'].split('.')))

    @property
    def fqn(self):
        """ Return fully qualified name."""
        return '@'.join([self._name, self.str_version])

    @property
    def version(self):
        """ Return bundle version."""
        return self._version

    @property
    def ydk_version(self):
        """ Return ydk version."""
        return self._ydk_version

    @property
    def str_version(self):
        """ Return string representation of version."""
        return "%s.%s.%s" % self.version

    @property
    def str_ydk_version(self):
        """ Return string representation of ydk version."""
        return "%s.%s.%s" % self.ydk_version


class BundleDependency(BundleDefinition):
    """ BundleDependency class represent a possible unresolved bundle, an extra attribute
    uri is added to locate this dependency file.

        uri (Remote_URI or Local_URI): URI for a remote bundle file or a local bundle file.
    """

    def __init__(self, data):
        super(BundleDependency, self).__init__(data)
        self.uri = parse_uri(data['uri'])


class Module(object):
    """ Module class for modules listed in bundle description file, it has following attributes:

        _name (str): module name.
        _revision (str): latest revision for this module.
        _kind (str): module type, could be 'MODULE' or 'SUBMODULE'.
        _uri (LocalURI or RemoteURI): URI to locate this module.

        Raises:
            KeyError if data if malformed.
    """
    __slots__ = ['_name', '_revision', '_kind', '_uri']

    def __init__(self, data):
        self._name = data['name']
        if 'revision' in data:
            self._revision = data['revision']
        else:
            self._revision = ''
        self._kind = data['kind']
        self._uri = parse_uri(data['uri'])

    @property
    def pkg_name(self):
        name = self._name
        name = name.replace('-', '_')
        if iskeyword(name):
            name = '%s_' % name
        if name[0] == '_':
            name = 'y%s' % name
        return name

    @property
    def fqn(self):
        """ Return fully qualified name."""
        return self._name + '@' + self._revision

    @property
    def uri(self):
        """ Return module uri."""
        return self._uri


class Bundle(BundleDefinition):
    """ Bundle class for a resolved bundle, it consumes a local bundle file, and
        has following additional attributes:

        modules (list of Module): list of modules defined in this bundle.
        dependencies (list of BundelDependencies): lsit of dependencies for
            this bundle, this could be an empty list.

        Raises:
            KeyError if data is malformed.
    """

    def __init__(self, uri):
        try:
            with open(uri) as json_file:
                data = json.load(json_file)
        except IOError as e:
            raise YdkGenException('Cannot open bundle file %s.' % uri)

        try:
            super(Bundle, self).__init__(data['bundle'])
            self.modules = []
            self.dependencies = []
            if 'modules' in data:
                for m in data['modules']:
                    self.modules.append(Module(m))
            if 'dependencies' in data['bundle']:
                for d in data['bundle']['dependencies']:
                    self.dependencies.append(BundleDependency(d))
        except KeyError as e:
            raise YdkGenException('Bundle file is not well formatted.')

        self.children = []


class Resolver(object):
    """ Bundle resolver class, it will resolve all the module files and bundle files
        referred to by current bundle file and its dependencies.

        output_dir (str) : output directory for generated API.
        cached_models_dir (str) : path to resolved model files.
        cached_bundles_dir(str) : path to resolved bundle files.
        tree (dict): dictionary to hold Bunlde instances.
        module_repos (nested defaultdict): holder for remote repositories.
        bundle_repos (dict): dictionary of temporary folders created, will be cleaned after
                                executing resolve function.

    """

    def __init__(self, output_dir, reuse_module=False, reuse_bundle=False):
        self.output_dir = output_dir

        cached_models_dir = os.path.join(output_dir, '.cache', 'models')
        cached_bundles_dir = os.path.join(output_dir, '.cache', 'bundles')

        if not reuse_module and os.path.isdir(cached_models_dir):
            rmtree(cached_models_dir)
        if not reuse_bundle and os.path.isdir(cached_bundles_dir):
            rmtree(cached_bundles_dir)
        if not os.path.isdir(cached_models_dir):
            os.makedirs(cached_models_dir)
        if not os.path.isdir(cached_bundles_dir):
            os.makedirs(cached_bundles_dir)

        self.cached_models_dir = cached_models_dir
        self.cached_bundles_dir = cached_bundles_dir

        self.tree = {}
        self.module_repos = dd()
        self.bundle_repos = {}

    def resolve(self, bundle_file):
        """ Resolve modules defined in bundle file and its dependency files,
        return list of Bundle instances, and folder for resolved modules.
        """
        uri = 'file://' + bundle_file
        bundle_file = self._resolve_bundle_file(parse_uri(uri))
        root = Bundle(bundle_file)
        self.tree[root.fqn] = root
        self._expand_tree(root)

        self._resolve_modules()
        self._clean_up()

        return self.tree.values(), self.cached_models_dir

    def _expand_tree(self, root):
        """ Populate uri to module_repos."""
        for m in root.modules:
            if isinstance(m.uri, Remote_URI):
                self.module_repos[m.uri.url][m.uri.commitid][m.uri.path] = m.fqn
            elif isinstance(m.uri, Local_URI):
                fname = os.path.basename(m.uri.url)
                dst = os.path.join(self.cached_models_dir, fname)
                logger.debug('Resolving module %s --> %s' % (fname, dst))
                copy(m.uri.url, dst)

        for d in root.dependencies:
            if d.fqn not in self.tree:
                file = self._resolve_bundle_file(d.uri)
                node = Bundle(file)
                self.tree[d.fqn] = node
            root.children.append(self.tree[d.fqn])
            self._expand_tree(node)

    def _resolve_bundle_file(self, uri):
        """ Resolve a remote or local bundle file, return the location for resolved file."""
        if isinstance(uri, Local_URI):
            src = uri.url
        elif isinstance(uri, Remote_URI):
            # fetch src from remote repository
            if uri.url not in self.bundle_repos:
                tmp_dir = tempfile.mkdtemp('.bundle')
                repo = Repo.clone_from(uri.url, tmp_dir)
                self.bundle_repos[uri.url] = Repo_Dir_Pair(repo, tmp_dir)

            repo = self.bundle_repos[uri.url].repo
            repo.git.checkout(uri.commitid)
            src = os.path.join(tmp_dir, uri.path)

        fname = os.path.basename(src)
        dst = os.path.join(self.cached_bundles_dir, fname)
        logger.debug('Resolving bundle %s --> %s' % (fname, dst))
        copy(src, dst)
        return dst

    def _resolve_modules(self):
        """ Resolve module files."""
        for url in self.module_repos:
            tmp_dir = tempfile.mkdtemp('.yang')
            logger.debug('Cloning from %s --> %s' % (url, tmp_dir))
            repo = Repo.clone_from(url, tmp_dir)
            for commitid in self.module_repos[url]:
                repo.git.checkout(commitid)
                for path in self.module_repos[url][commitid]:
                    module_fqn = self.module_repos[url][commitid][path]
                    fname = module_fqn + '.yang'
                    dst = os.path.join(self.cached_models_dir, fname)
                    logger.debug('Resolving module %s --> %s' % (module_fqn, dst))
                    copy(os.path.join(tmp_dir, path),
                        os.path.join(self.cached_models_dir, fname))
            logger.debug('Cleaning folder %s' % tmp_dir)
            rmtree(tmp_dir)

    def _clean_up(self):
        """ Remove all temporary directories created while cloning repositories."""
        for url in self.bundle_repos:
            repo_dir = self.bundle_repos[url]
            logger.debug('Cleaning folder %s' % repo_dir.dir)
            rmtree(repo_dir.dir)

if __name__ == '__main__':

    import doctest
    doctest.testmod()
