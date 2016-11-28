.. _ref-nmspcore:


ydk\:\:path
===============================

Data Structures
---------------

.. cpp:namespace:: ydk::path

+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`Annotation <Annotation>`                                     |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`Capability <Capability>`                                     |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`CodecService <CodecService>`                                 |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`DataNode <DataNode>`                                         |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`DiagnosticNode <DiagnosticNode>`                             |
|        | | Template class for Diagnostic.                                           |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`ModelProvider <ModelProvider>`                               |
|        | | Interface for all ModelProvider implementations.                         |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`Repository <Repository>`                                     |
|        | | Represents the Repository of YANG models.                                |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`RootSchemaNode <RootSchemaNode>`                             |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`Rpc <Rpc>`                                                   |
|        | | An instance of the YANG schmea rpc node.                                 |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`SchemaConstraint <SchemaConstraint>`                         |
|        | | YANG validity restriction (must, length, etc) structure providing        |
|        | | information from the schema.                                             |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`SchemaNode <SchemaNode>`                                     |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueBinaryType <SchemaValueBinaryType>`               |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueBitsType <SchemaValueBitsType>`                   |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueBoolType <SchemaValueBoolType>`                   |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueDec64Type <SchemaValueDec64Type>`                 |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueEmptyType <SchemaValueEmptyType>`                 |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueEnumerationType <SchemaValueEnumerationType>`     |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueIdentityType <SchemaValueIdentityType>`           |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueInstanceIdType <SchemaValueInstanceIdType>`       |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueLeafrefType <SchemaValueLeafrefType>`             |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueNumType <SchemaValueNumType>`                     |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueStringType <SchemaValueStringType>`               |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueType <SchemaValueType>`                           |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`SchemaValueUnionType <SchemaValueUnionType>`                 |
+--------+----------------------------------------------------------------------------+
| class  | | :cpp:class:`ServiceProvider <ServiceProvider>`                           |
|        | | Interface for all ServiceProvider implementations.                       |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`Statement <Statement>`                                       |
|        | | Represents the YANG Statement                                            |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`ValidationService <ValidationService>`                       |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`YCPPCoreError <YCPPCoreError>`                         |
|        | | Base class for YDK Exceptions.                                           |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`YCPPDataValidationError <YCPPDataValidationError>`     |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`YCPPPathError <YCPPPathError>`                         |
+--------+----------------------------------------------------------------------------+
| struct | | :cpp:class:`YCPPSchemaValidationError <YCPPSchemaValidationError>` |
+--------+----------------------------------------------------------------------------+

Enumerations
------------

ValidationError
~~~~~~~~~~~~~~~
:ref:`ValidationError <ref-validationerror>`

+------------------------+--------------------------------------------------------------------------+
| enumerator             | | documentation                                                          |
+------------------------+--------------------------------------------------------------------------+
| SUCCESS                | | No error                                                               |
+------------------------+--------------------------------------------------------------------------+
| SCHEMA_NOT_FOUND       | | Entity's schema node is not found                                      |
+------------------------+--------------------------------------------------------------------------+
| INVALID_USE_OF_SCHEMA  | | If element cannot have children as per schema (leaf, leaf-list, anyxml)|
+------------------------+--------------------------------------------------------------------------+
| TOOMANY                | | Too many instances of some object                                      |
+------------------------+--------------------------------------------------------------------------+
| DUPLEAFLIST            | | Multiple instances of leaf-list                                        |
+------------------------+--------------------------------------------------------------------------+
| DUPLIST                | | Multiple instances of list                                             |
+------------------------+--------------------------------------------------------------------------+
| NOUNIQ                 | | Unique leaves match on 2 list instances (data)                         |
+------------------------+--------------------------------------------------------------------------+
| OBSDATA                | | Obsolete data instantiation (data)                                     |
+------------------------+--------------------------------------------------------------------------+
| NORESOLV               | | No resolvents found for an expression (data)                           |
+------------------------+--------------------------------------------------------------------------+
| INELEM                 | | Nvalid element (data)                                                  |
+------------------------+--------------------------------------------------------------------------+
| MISSELEM               | | Missing required element (data)                                        |
+------------------------+--------------------------------------------------------------------------+
| INVAL                  | | Invalid value of an element (data)                                     |
+------------------------+--------------------------------------------------------------------------+
| INVALATTR              | | Invalid attribute value (data)                                         |
+------------------------+--------------------------------------------------------------------------+
| INATTR                 | | Invalid attribute in an element (data)                                 |
+------------------------+--------------------------------------------------------------------------+
| MISSATTR               | | Missing attribute in an element (data)                                 |
+------------------------+--------------------------------------------------------------------------+
| NOCONSTR               | | Value out of range/length/pattern (data)                               |
+------------------------+--------------------------------------------------------------------------+
| INCHAR                 | | Unexpected characters (data)                                           |
+------------------------+--------------------------------------------------------------------------+
| INPRED                 | | Predicate resolution fail (data)                                       |
+------------------------+--------------------------------------------------------------------------+
| MCASEDATA              | | Data for more cases of a choice (data)                                 |
+------------------------+--------------------------------------------------------------------------+
| NOMUST                 | | Unsatisfied must condition (data)                                      |
+------------------------+--------------------------------------------------------------------------+
| NOWHEN                 | | Unsatisfied when condition (data)                                      |
+------------------------+--------------------------------------------------------------------------+
| INORDER                | | Invalid order of elements (data)                                       |
+------------------------+--------------------------------------------------------------------------+
| INWHEN                 | | Irresolvable when condition (data)                                     |
+------------------------+--------------------------------------------------------------------------+
| NOMIN                  | | Min-elements constraint not honored (data)                             |
+------------------------+--------------------------------------------------------------------------+
| NOMAX                  | | Max-elements constraint not honored (data)                             |
+------------------------+--------------------------------------------------------------------------+
| NOREQINS               | | Required instance does not exits (data)                                |
+------------------------+--------------------------------------------------------------------------+
| NOLEAFREF              | | Leaf pointed to by leafref does not exist (data)                       |
+------------------------+--------------------------------------------------------------------------+
| NOMANDCHOICE           | | No mandatory choice case branch exists (data)                          |
+------------------------+--------------------------------------------------------------------------+
| INVALID_BOOL_VAL       | | Invalid boolean value                                                  |
+------------------------+--------------------------------------------------------------------------+
| INVALID_EMPTY_VAL      | | Invalid empty value                                                    |
+------------------------+--------------------------------------------------------------------------+
| INVALID_PATTERN        | | Pattern did not match                                                  |
+------------------------+--------------------------------------------------------------------------+
| INVALID_LENGTH         | | Length is invalid                                                      |
+------------------------+--------------------------------------------------------------------------+
| INVALID_IDENTITY       | | Invalid identity                                                       |
+------------------------+--------------------------------------------------------------------------+
| INVALID_ENUM           | | Invalid enumeration                                                    |
+------------------------+--------------------------------------------------------------------------+
