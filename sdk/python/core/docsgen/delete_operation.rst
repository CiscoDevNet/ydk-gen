Delete operation
****************
This document explains how to use YDK object to delete nodes. Examples below use `ydktest.json <https://raw.githubusercontent.com/CiscoDevNet/ydk-gen/master/profiles/test/ydktest.json>`_ profile file to generate YDK test package.

Let's write some boilerplate code for device connection:

.. code-block:: python

    from ydk.services import CRUDService
    from ydk.providers import NetconfServiceProvider
    from ydk.models import ydktest_filterread as ysanity
    ncc = NetconfServiceProvider(address='127.0.0.1',
                                 username='admin',
                                 password='admin',
                                 protocol='ssh',
                                 port=12022)
    crud = CRUDService()

Delete operation could be executed on YANG container, list and items in  YANG list or leaf-list, to delete a container:

.. code-block:: python

	runner = ysanity.Runner()
	crud.delete(ncc, runner)

to delete a list:

.. code-block:: python

    runner = ysanity.Runner()
    runner.one.name = 'one'
    foo = ysanity.Runner.OneList.Ldata()
    bar = ysanity.Runner.OneList.Ldata()
    foo.number = 1
    foo.name = 'foo'
    bar.number = 2
    bar.name = 'bar'
    baz.number = 1
    baz.name = 'baz'
    runner.one_list.ldata.extend([foo, bar, baz])
	crud.delete(ncc, runner.one_list.ldata)

to delete a slice of above list:

.. code-block:: python

	crud.delete(ncc, runner.one_list.ldata[1:])

the same syntax could be used to delete items in leaf-list.






