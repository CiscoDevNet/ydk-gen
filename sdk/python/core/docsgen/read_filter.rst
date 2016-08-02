Read Using Object Filter
************************
In read operation, YDK object is used as read filter. This document explains how to use YDK object as a read filter. Examples below use `ydktest.json <https://raw.githubusercontent.com/CiscoDevNet/ydk-gen/master/profiles/test/ydktest.json>`_ profile file to generate YDK test package.
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

and :py:class:`ydk.services.CodecService` to simplify payload comparison:

.. code-block:: python

    from ydk.services.codec_service import CodecService
    from ydk.providers.codec_provider import CodecServiceProvider
    codec = CodecService()
    codec_provider = CodecServiceProvider(type='xml')

and configure the device with the initial data below:

.. code-block:: python

    a = ysanity.A()
    a.a1, a.a2, a.a3 = "some value", "value of a2", "value of a3"
    a.b.b1, a.b.b2, a.b.b3 = "some value", "value of b2", "value of b3"
    a.b.f = a.b.F()
    a.b.f.f1 = 'f'
    a.b.c = a.b.C()
    a.b.d.d1 = "some value d1"
    a.b.d.d2 = "value of d2"
    a.b.d.d3 = "value of d3"
    a.b.d.e.e1, a.b.d.e.e2 = "some value e1", "value of e2"
    l1, l2, l3 = a.Lst(), a.Lst(), a.Lst()
    l1.number, l1.value = 1, "one"
    l2.number, l2.value = 2, "two"
    l3.number, l3.value = 3, "three"
    a.lst.extend([l1, l2, l3])

    crud.create(ncc, a)

The configuration above will config following data in device:

.. code-block:: xml

    <a xmlns="http://cisco.com/ns/yang/ydk-filter">
      <a1>some value</a1>
      <a2>value of a2</a2>
      <a3>value of a3</a3>
      <b>
        <b1>some value</b1>
        <b2>value of b2</b2>
        <b3>value of b3</b3>
        <c/>
        <d>
          <d1>some value d1</d1>
          <d2>value of d2</d2>
          <d3>value of d3</d3>
          <e>
            <e1>some value e1</e1>
            <e2>value of e2</e2>
          </e>
        </d>
        <f>
          <f1>f</f1>
        </f>
      </b>
      <lst>
        <number>1</number>
        <value>one</value>
      </lst>
      <lst>
        <number>2</number>
        <value>two</value>
      </lst>
      <lst>
        <number>3</number>
        <value>three</value>
      </lst>
    </a>

where `<c>` and `<f>` are presence container.

Read everything
===============
The simplest filter is the top level container:

.. code-block:: python

    a_read = crud.read(ncc, ysanity.A())
    print codec.encode(codec_provider, a_read)

the top level container filters nothing and return every data under current level:

.. code-block:: xml

    <a xmlns="http://cisco.com/ns/yang/ydk-filter">
      <a1>some value</a1>
      <a2>value of a2</a2>
      <a3>value of a3</a3>
      <b>
        <b1>some value</b1>
        <b2>value of b2</b2>
        <b3>value of b3</b3>
        <c/>
        <d>
          <d1>some value d1</d1>
          <d2>value of d2</d2>
          <d3>value of d3</d3>
          <e>
            <e1>some value e1</e1>
            <e2>value of e2</e2>
          </e>
        </d>
        <f>
          <f1>f</f1>
        </f>
      </b>
      <lst>
        <number>1</number>
        <value>one</value>
      </lst>
      <lst>
        <number>2</number>
        <value>two</value>
      </lst>
      <lst>
        <number>3</number>
        <value>three</value>
      </lst>
    </a>


Filter out more stuff
=====================

To make the filter more strict, you could assign more value to it. For example, if you are only interested in presence container `C`:

.. code-block:: python

    a = ysanity.A()
    a.b.c = a.b.C()
    a_read = crud.read(ncc, a)
    print codec.encode(a_read)

.. code-block:: xml

    <a xmlns="http://cisco.com/ns/yang/ydk-filter">
      <b>
        <c/>
      </b>
    </a>


Content match nodes
===================
According to `NETCONF RFC <https://tools.ietf.org/html/rfc6241#section-6.2.5>`_, a "content match node" is used to select sibling nodes for filter output. Let's try this concept with the following example:

.. code-block:: python

    a = ysanity.A()
    a.b.b1 = "some value"
    a_read = crud.read(ncc, a)
    print codec.encode(codec_provider, a_read)

In the example show above, the `a.b.b1` leaf serves as a content match node, therefore its siblings `<b2>` , `<b3>`, `<c>`, `<d>`, `<f>` and their children are all being kept.

.. code-block:: xml

    <a xmlns="http://cisco.com/ns/yang/ydk-filter">
      <b>
        <b1>some value</b1>
        <b2>value of b2</b2>
        <b3>value of b3</b3>
        <c/>
        <d>
          <d1>some value d1</d1>
          <d2>value of d2</d2>
          <d3>value of d3</d3>
          <e>
            <e1>some value e1</e1>
            <e2>value of e2</e2>
          </e>
        </d>
        <f>
          <f1>f</f1>
        </f>
      </b>
    </a>


Read on leaf
============
YDK also provides you with a `READ` class that could be used to read the value on a particular leaf. Let's use this `READ` class and import it from `ydk.types`:

.. code-block:: python

    from ydk.types import READ
    a = ysanity.A()
    a.a1 = READ()
    a_read = crud.read(ncc, a)
    print codec.encode(codec_provider, a_read)

.. code-block:: xml

    <a xmlns="http://cisco.com/ns/yang/ydk-filter">
      <a1>some value</a1>
    </a>
