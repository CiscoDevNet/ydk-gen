Read Using Object Filter
************************
This document will explain and give examples to read operation using object filter.
Types of read operation explained will include:

- Read everything.
- Read using READ class.
- Read on lists.

Structure of example object
===========================

The example will based on the structure and data defined below. 

- The structure of A is:
.. code-block:: xml

   +--rw a
      +--rw a1  string
      +--rw a2  string
      +--rw a3  string
      +--rw b
      |  +--rw b1   string
      |  +--rw b2   string
      |  +--rw b3   string
      |  +--rw c   
      |  +--rw d
      |     +--rw d1  string
      |     +--rw d1  string
      |     +--rw d1  string
      |     +--rw e
      |           +--rw e1 string
      |           +--rw e2 string
      +--rw lst* [number]
         +--rw number int32
         +--value string

- The data stored in the device is:

.. code-block:: xml

    <a>
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

Read everything
===============
To read everything, simply initiated an instance of top level object A.

.. code-block:: python

    obja = A()

This will be encoded as,

.. code-block:: xml

    <filter type="subtree">
        <a/>
    </filter>

And the exptected return value should be,

.. code-block:: xml

    <a>
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


Read using READ class
=====================
Explicitly assign READ instance to object leaf will get a more strcting filter, cases include:

- Read on leaf.
    To read on a particular leaf, assign READ instance to that attribute:

    .. code-block:: python

        obja = A()
        obja.a1 = READ()

    This will be translated to:

    .. code-block:: xml

        <filter type="subtree">
            <a>
                </a1>
            </a>
        </filter>

    The expected return value will only contain value for this leaf:

    .. code-block:: xml

        <a>
          <a1>some value</a1>
        </a>

- Read on presence container.
    To read a presence container like c, assign instance of READ class to it.

    .. code-block:: python

        obja = A()
        c = obja.b.c
        c = READ()

    This will be encoded as:

    .. code-block:: xml

        <filter type="subtree">
            <a>
                <b>
                    <c/>
                </b>
            </a>
        </filter>
    And the expected return value for this should be:

    .. code-block:: xml

        <a>
          <b>
            <c/>
          </b>
        </a>

- Read on nested container.
    Assign instance of READ class will make filter more strict, below are two examples.

    - Example1:

        If the filter is configured as follow,

        .. code-block:: python

            obja = A()
            obja.b.b1 = "some value"

        Then it will be encoded as:

        .. code-block:: xml

            <filter type="subtree">
                <a>
                    <b>
                        <b1>some value</b1>
                    </b>
                </a>
            </filter> 

        Since the value of ``<b1>`` matches the value stored in the device, it will return all the value stored in the nested container under ``<b>``:

        .. code-block:: xml

            <a>
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
              </b>
            </a>

    - Example2:

        If the filter is set as follows,

        .. code-block:: python

            obja = A()
            e = obja.b.d.e
            e.e1 = "some value e1"
            d = obja.b.d
            d.d1 = "some value d1"

        The filter will be encoded as,

        .. code-block:: xml

            <filter type="substree">
                <a>
                    <b>
                        <d>
                            <d1>some value d1</d1>
                            <e>
                                <e1>some value e1</e1>
                            </e>
                        </d>
                    </b>
                </a>
            </filter>


        And the expected return value should be,

        .. code-block:: xml

            <a>
              <b>
                <d>
                  <d1>some value d1</d1>
                  <e>
                    <e1>some value e1</e1>
                    <e2>value of e2</e2>
                  </e>
                </d>
              </b>
            </a>

Read on lists
=============
To read on list, assign key value that matches the value in the device, if the filter is configured as follows,

.. code-block:: python

    l1, l2 = A.Lst(), A.Lst()
    l1.number, l2.number =  1, 2
    obja = A()
    obja.lst.extend([l1, l2])

It will be encoded as,

.. code-block:: xml

    <filter type="subtree">
        <a>
            <lst>
                <number>1</number>
                <value>one</value>
            </lst>
            <lst>
                <number>2</number>
                <value>two</value>
            </lst>
        </a>
    </filter>

And the exptected return value should be,

.. code-block:: xml

    <a>
      <lst>
        <number>1</number>
        <value>one</value>
      </lst>
      <lst>
        <number>2</number>
        <value>two</value>
      </lst>
    </a>