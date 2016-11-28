Annotation
==========


.. cpp:namespace:: ydk::path

.. cpp:class:: Annotation

Class represents an annotation.

An annotation has a namespace and a name and an associated value.
Annotations are not defined in the YANG model and hence just provide a means of hanging some useful data to :cpp:class:`DataNode<DataNode>`. For example netconf edit-config rpc operation uses the annotation ``nc:operation`` (``nc`` refers to the netconf namespace) on the data nodes to describe the kind of operation one needs to perform on the given :cpp:class:`DataNode<DataNode>`.

    .. cpp:member:: std::string m_ns

        Annotation namespace.

    .. cpp:member:: std::string m_name

        Annotation name.

    .. cpp:member:: std::string m_val

        Annotation value.

    .. cpp:function:: void Annotation(const std::string& ns,\
                        const std::string& name,\
                            const std::string& val)


    .. cpp:function:: void Annotation(const Annotation& an)


    .. cpp:function:: void Annotation(Annotation&& an)


    .. cpp:function:: Annotation& operator=(const Annotation& an)


    .. cpp:function:: Annotation& operator=(Annotation&& an)


    .. cpp:function:: bool operator==(const Annotation& an) const
