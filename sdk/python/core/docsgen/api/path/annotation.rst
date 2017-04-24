Annotation
==========

.. module:: ydk.path
    :synopsis: Path API' Annotation

.. py:class:: Annotation(namespace, name, value)

    An annotation has a namespace and a name and an associated value. Annotations are not defined in the YANG model and hence just provide a means of hanging some useful data to :py:class:`DataNode<ydk.path.DataNode>`. For example netconf edit-config rpc operation uses the annotation ``nc:operation`` (``nc`` refers to the netconf namespace) on the data nodes to describe the kind of operation one needs to perform on the given :py:class:`DataNode<ydk.path.DataNode>`.

    :param namespace: (``str``) Annotation namespace.
    :param name: (``str``) Annotation name.
    :param value: (``str``) Annotation value.

    Example usage:

    .. code-block:: python

        >>> from ydk.path import Annotation
        >>> ann = Annotation('ietf-netconf', 'operation', 'merge')
