//
// @file path_api.hpp
// @brief The main ydk public header.
//
// YANG Development Kit
// Copyright 2016 Cisco Systems. All rights reserved
//
////////////////////////////////////////////////////////////////
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//////////////////////////////////////////////////////////////////

#ifndef YDK_CORE_HPP
#define YDK_CORE_HPP

#include <string>
#include <vector>
#include <algorithm>
#include "errors.hpp"
#include "types.hpp"
#include "validation_service.hpp"

namespace ydk {
namespace path {

///
/// @mainpage About
///
/// A generic API for working with YANG modelled data.
///
/// @section about-features Main Features
///
/// - A ServiceProvider interface for ServiceProviders
/// - A Repository class to help create a SchemaNode Tree based on capabilities.
/// - Creation of DataNode Tree's for config and oper
/// - Create and Invoke RPC's
/// - ValidationService that validates DataNode Tree's based on criteria.
/// - CodecService that can encode a DataNode to XML/JSON and decode from XML/JSON to a dataNode tree
///
/// @section about-license License
///
/// YANG Development Kit
/// Copyright 2016 Cisco Systems. All rights reserved
///
///
/// Licensed to the Apache Software Foundation (ASF) under one
/// or more contributor license agreements.  See the NOTICE file
/// distributed with this work for additional information
/// regarding copyright ownership.  The ASF licenses this file
/// to you under the Apache License, Version 2.0 (the
/// "License"); you may not use this file except in compliance
/// with the License.  You may obtain a copy of the License at
///
///   http://www.apache.org/licenses/LICENSE-2.0
///
///  Unless required by applicable law or agreed to in writing,
/// software distributed under the License is distributed on an
/// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
/// KIND, either express or implied.  See the License for the
/// specific language governing permissions and limitations
/// under the License.
///


///
/// @page howto How To ...
///
/// - @subpage howtoErrors
/// - @subpage howtoserviceprovider
/// - @subpage howtoschemas
/// - @subpage howtodata
/// - @subpage howtomemory
/// - @subpage howtopath
/// - @subpage howtorpc
/// - @subpage howtovalidation
/// - @subpage howtocodec
/// - @subpage howtologger
///

///
/// @page howtoErrors Exceptions and Error Handling.
/// TODO
///

///
/// @page howtoserviceprovider ServiceProvider.
/// A ServiceProvider extends the class ydk::ServiceProvider
/// and provides an interface to obtain the root SchemaTree
/// based on the set of Capability(s) supported by it.
///
/// The ServiceProvider is also responsible for Servicing RPC's.
///
/// @section ServiceProvider Errors
/// TODO
///
/// @section Capability
/// A capability is a tuple that contains
/// - module-name
/// - revision
/// - set of enabled features
/// - set of deviations active on this module
///
/// @note to ServiceProvider implementors
/// Use the Repository class to instantiate a SchemaTree based on the Capabilities.
///
///

///
/// @page howtoschemas SchemaNode Tree
/// Talk about SchemaNodeTree , RootSchemaNodeTree.
/// Memory management of trees. DO NOT get rid of the SchemaTree if there are DataTree's referencing it
/// Thread safety
/// Inspecting YANG meta data
/// Traversing the hierarchy (iterations and find)
///

///
/// @page howtodata DataNode Tree
/// How to create DataNode Trees
/// Memory management
/// Quirks
/// Accessing the Schema Tree
/// Traversing the hierarchy (iteration and find)
///

///
/// @page howtomemory
/// Node containment hierarchies
/// Best practices
/// TODO

/// @page howtopath Path Syntax.
/// Full XPath notation is supported for find operations on DataNode(s). This XPath conforms to the YANG specification
/// (RFC 6020 section 6.4). Some useful examples:
///
/// - get all top-level nodes of the __module-name__
///
/// /module-name:*
///
/// - get all the descendants of __container__ (excluding __container__)
///
///  /module-name:container//\asterisk
///
/// - get __list__ instance with __key1__ of value __1__ and
///    __key2__ of value __2__ (this can return more __list__ instances if there are more keys
///    than __key1__ and __key2__)
///
/// /module-name:container/list[key1='1'][key2='2']
///
/// - get __leaf-list__ instance with the value __val__
///
/// /module-name:container/leaf-list[.='val']
///
/// - get __aug-leaf__, which was added to __module-name__ from an augment module __augment-module__
///
/// /module-name:container/container2/augment-module:aug-cont/aug-leaf
///
///
/// A very small subset of this full XPath is recognized by DataNode#create. Basically, only a relative or absolute
/// path can be specified to identify a new data node. However, lists must be identified by all their keys and created
/// with all of them, so for those cases predicates are allowed. Predicates must be ordered the way the keys are
/// ordered and all the keys must be specified. Every predicate includes a single key with its value. Optionally,
/// leaves and leaf-lists can have predicates specifying their value in the path itself. All these paths are valid
/// XPath expressions. Example: (Relative to Root Data or RootSchemaNode)
///
/// ietf-yang-library:modules-state/module[name='ietf-yang-library'][revision='']/conformance[.='implement']
///
/// Almost the same XPath is accepted by SchemaNode methods. The difference is that it is not used on data, but schema,
/// which means there are no key values and only one node matches one path. In effect, lists do not have to have any
/// predicates. If they do, they do not need to have all the keys specified and if values are included, they are
/// ignored. Nevertheless, any such expression is still a valid XPath, but can return more nodes if executed on a
/// data tree.
/// Examples (all returning the same node):
///
/// ietf-yang-library:modules-state/module/submodules
/// ietf-yang-library:modules-state/module[name]/submodules
/// ietf-yang-library:modules-state/module[name][revision]/submodules
/// ietf-yang-library:modules-state/module[name='ietf-yang-library'][revision]/submodules
///
///
/// Also note, that in all cases the node's prefix is specified as the name of the appropriate YANG schema. Any node
/// can be prefixed by the module name. However, if the prefix is omitted, the module name is inherited from the
/// previous (parent) node. It means, that the first node in the path is always supposed to have a prefix.
///

///
/// @page howtorpc Rpc
/// An Rpc represents an instance of the YANG Rpc schema node.
/// To invoke and Rpc the user first creates and Rpc using the RootSchema#create_rpc call passing in
/// a path with the name of the rpc For example auto get_config = root_schema->create_rpc("ietf-netconf:get-config")
///
/// The input DataNode can be obtained using Rpc#input. This can be used to populate/create the child
/// nodes of input as per this rpc's schema.
/// The Rpc is a callable that takes  a single argument which is the ServiceProvider. To invoke the rpc
/// do this
/// auto config = get_config(sp); /// sp is a service provider
/// The config variable above is the DataNode representing the output of the rpc
///
///

///
/// @page howtovalidation Validation
/// DataNode Tree can be validated using the ValidationService
///



///
/// @page howtocodec Encoding and Decoding.
/// A given DataNode Tree can be encoded and decoded into a variety of formats using the CodecService.
//
/// DataNode Tree can be validated using the ValidationService
///

///
/// @page howtologger
/// TODO
///

// Forward References
class DataNode ;
class Rpc;
class SchemaNode ;
class RootSchemaNode ;

///
/// @brief Validation Service
///
/// Instances of this class can validate the DataNode Tree based
/// on the option supplied.
///
class ValidationService
{
public:
    ValidationService();
    ~ValidationService();

    ///
    /// @brief validates dn based on the option
    ///
    /// @param[in] dn The root of DataNode tree to validate.
    /// @param[in] option The context for validation.
    /// @throws YCPPValidationError if validation errors were detected.
    /// @throws YCPPInvalidArgumentError if the arguments are invalid.
    ///
    void validate(const DataNode & dn, ydk::ValidationService::Option option);
};

///
/// @brief CodecService
///
/// The Encode/Decode Service.
///
class CodecService
{
public:
    CodecService();
    ~CodecService();

    ///
    /// @brief encode the given DataNode Tree
    ///
    /// @param[in] dn The DataNode to encode
    /// @param[in] format to encode to.
    /// @param[in] pretty if true the output is indented for human consumption.
    /// @return The encoded string.
    //  @throws YCPPInvalidArgumentError if the arguments are invalid.
    ///
    std::string encode(const DataNode & dn, EncodingFormat format, bool pretty);

    ///
    /// @brief decode the buffer to return a DataNode
    ///
    /// @param[in] root_schema The root schema to use.
    /// @param[in] buffer The string representation of the DataNode.
    /// @param[in] format .Note ::TREE is not supported.
    /// @return The DataNode instantiated or nullptr in case of error.
    /// @throws YCPPInvalidArgumentError if the arguments are invalid.
    ///
    std::shared_ptr<DataNode> decode(const RootSchemaNode & root_schema, const std::string& buffer, EncodingFormat format);
};


///
/// @brief Base class for YCPP Errors
///
/// The subclasses give a specialized view of the error that has occurred.
///
struct YCPPCoreError : public ydk::YCPPError
{
    YCPPCoreError();

    YCPPCoreError(const std::string& msg);

};


///
/// @brief Error that encapsualtes the validation errors
///        on a data tree
///
struct YCPPDataValidationError : public YCPPCoreError
{
    /// Data Validation Error Enum
    enum class Error {
        SUCCESS,  /// no error

        TOOMANY,      /// too many instances of some object
        DUPLEAFLIST,  /// multiple instances of leaf-list
        DUPLIST,      /// multiple instances of list
        NOUNIQ,       /// unique leaves match on 2 list instances (data)
        OBSDATA,      /// obsolete data instantiation (data) */
        /* */
        NORESOLV,     /// no resolvents found for an expression (data) */
        INELEM,       /// nvalid element (data) */
        /* */
        MISSELEM,     /// missing required element (data) */
        INVAL,        /// invalid value of an element (data) */
        INVALATTR,    /// invalid attribute value (data) */
        INATTR,       /// invalid attribute in an element (data) */
        MISSATTR,     /// missing attribute in an element (data) */
        NOCONSTR,     /// value out of range/length/pattern (data) */
        INCHAR,       /// unexpected characters (data) */
        INPRED,       /// predicate resolution fail (data) */
        MCASEDATA,    /// data for more cases of a choice (data) */
        NOMUST,       /// unsatisfied must condition (data) */
        NOWHEN,       /// unsatisfied when condition (data) */
        INORDER,      /// invalid order of elements (data) */
        INWHEN,      /// irresolvable when condition (data) */
        NOMIN,        /// min-elements constraint not honored (data) */
        NOMAX,        /// max-elements constraint not honored (data) */
        NOREQINS,     /// required instance does not exits (data) */
        NOLEAFREF,    /// leaf pointed to by leafref does not exist (data) */
        NOMANDCHOICE, /// no mandatory choice case branch exists (data) */


    };

    YCPPDataValidationError();

    /// List of pair<DataNode, ValidationError>. The Validation Error is specific to
    /// this node
    std::vector<std::pair<DataNode*,Error>> errors;

};

struct YCPPPathError : public YCPPCoreError
{
    enum class Error {
        SUCCESS,  /// no error

        XPATH_INTOK,  /// unexpected XPath token
        XPATH_EOF,    /// unexpected end of an XPath expression
        XPATH_INOP,   /// invalid XPath operation operands
        /* */
        XPATH_INCTX,  /// invalid XPath context type

        PATH_INCHAR,  /// invalid characters (path)
        PATH_INMOD,   /// invalid module name (path)
        PATH_MISSMOD, /// missing module name (path)
        PATH_INNODE,  /// invalid node name (path)
        PATH_INKEY,   /// invalid key name (path)
        PATH_MISSKEY, /// missing some list keys (path)
        PATH_EXISTS,  /// target node already exists (path)
        PATH_MISSPAR, /// some parent of the target node is missing (path)
        PATH_AMBIGUOUS //// thrown in create where the path expression cannot uniquely identify a given node
    };

    Error err;

    YCPPPathError(YCPPPathError::Error error_code);

};

struct YCPPCodecError : public YCPPCoreError
{
    enum class Error {
        SUCCESS,  /// no error

        XML_MISS,     ///  missing XML object
        XML_INVAL,    ///  invalid XML object
        XML_INCHAR,   /// invalid XML character

        EOF_ERR,      /// unexpected end of input data

    };

    Error err;

    YCPPCodecError(YCPPCodecError::Error merror);
};

///
/// @brief Annotation
///
/// Class represents an annotation.
/// An annotation has a namespace and a name and an associated value.
/// Annotations are not defined in the YANG model and hence just provide a means of hanging
/// some useful data to DataNodes. For example netconf edit-config rpc operation uses
/// the annotation nc:operation (nc refers to the netconf namespace) on the data nodes
/// to describe the kind of operation one needs to perform on the given DataNode.
///
struct Annotation{

    Annotation(const std::string& ns, const std::string& name, const std::string& val);

    Annotation(const Annotation& an);

    Annotation(Annotation&& an);

    Annotation& operator=(const Annotation& an);

    Annotation& operator=(Annotation&& an);

    bool operator==(const Annotation& an) const;

    std::string m_ns;
    std::string m_name;
    std::string m_val;

};


///
/// @brief represents the YANG Statement
///
struct Statement {

    Statement();

    Statement(const std::string& mkeyword, const std::string& marg);


    Statement(const Statement& stmt);

    Statement(Statement&& stmt);

    ~Statement();

    Statement& operator=(const Statement& stmt);

    Statement& operator=(Statement&& stmt);


    /// YANG keyword corresponding to the Statement
    std::string keyword;
    /// the arg if any
    std::string  arg;

};

///
/// @brief Represents a Node in the SchemaTree.
///
class SchemaNode
{

public:
    ///
    /// @brief The destructor.
    ///
    /// Note a SchemaNode represents a containment hierarchy. So
    /// invocation of the destructor will lead to the children of this
    /// node being destroyed.
   virtual ~SchemaNode();

    ///
    /// @brief returns the XPath expression of this Node in the NodeTree
    ///
    /// Get the path expression representing this Node in in the NodeTree.
    /// @return std::string representing the path to this Node.
    ///
    virtual std::string path() const = 0;

    ///
    /// @brief finds descendant nodes that match the given xpath expression
    ///
    /// This API finds descendant nodes in the Schema tree that satisfy
    /// the given path expression. See @see howtopath
    /// @param path The path expression.
    /// @return vector of SchemaNode  that satisfies the criterion.
    /// @throws YCPPPathError if the path expression in invalid, See error code for details.
    /// @throws YCPPInvalidArgumentError if the argument is invalid.
    ///
    virtual std::vector<SchemaNode*> find(const std::string& path) const = 0;

    ///
    /// @brief get the Parent Node of this SchemaNode in the tree.
    ///
    /// Returns the parent Node of this SchemaNode in the tree
    /// @return pointer the parent Node or nullptr in case this is the root.
    ///
    virtual const SchemaNode* parent() const noexcept = 0 ;

    ///
    /// @brief return the children of this SchemaNode in the NodeTree.
    ///
    /// Returns the children of this SchemaNode.
    ///@return the children of this node.
    virtual const std::vector<std::unique_ptr<SchemaNode>> & children() const = 0;

    ///
    /// @brief get the root of NodeTree this node is part of
    ///
    /// Returns the root of the NodeTree this nodes is part of
    /// @return the pointer to the root
    ///
    virtual const SchemaNode& root() const noexcept = 0;

    ///
    /// @brief return the YANG statement associated with this SchemaNode
    ///
    /// Returns the YANG statement associated with this SchemaNode
    /// @return the yang statement for this SchemaNode
    ///
    virtual Statement statement() const = 0;

    ///
    /// @brief return YANG statement corresponding the the keys
    ///
    /// Returns the vector of Statement keys
    /// @return vector of Statement that represent keys
    ///
    virtual std::vector<Statement> keys() const = 0;

};

///
/// @brief The RootSchemeNode.
///
/// Instances of this class represent the Root of the SchemaTree.
/// A RootSchemaNode can be used to instantiate a DataNodeTree or an RPC object
/// The children of the RootSchemaNode represent the top level SchemaNodes
/// in the YANG module submodules.
///
class RootSchemaNode : public SchemaNode
{
public:
    ///
    /// @brief Destructor for the RootSchemaNode
    ///
   virtual ~RootSchemaNode();

   std::string path() const;

    ///
    /// @brief finds descendant nodes that match the given xpath expression
    ///
    /// This API finds descendant nodes in the Schema tree that satisfy
    /// the given path expression. See @see howtopath
    /// @param path The path expression.
    /// @return vector of SchemaNode  that satisfies the criterion.
    /// @throws YCPPPathError if the path expression in invalid, See error code for details.
    /// @throws YCPPInvalidArgumentError if the argument is invalid.
    ///
    virtual std::vector<SchemaNode*> find(const std::string& path) const = 0;

    ///
    /// @brief get the Parent Node of this SchemaNode in the tree.
    ///
    /// Returns the parent Node of this SchemaNode in the tree
    /// @return pointer the parent Node or nullptr in case this is the root.
    ///
   virtual SchemaNode* parent() const noexcept;

    ///
    /// @brief return the children of this SchemaNode in the NodeTree.
    ///
    /// Returns the children of this SchemaNode.
    ///@return the children of this node.
    virtual const std::vector<std::unique_ptr<SchemaNode>>& children() const = 0;

    ///
    /// @brief get the root of NodeTree this node is part of
    ///
    /// Returns the root of the NodeTree this nodes is part of
    /// @return the pointer to the root
    ///
    virtual const SchemaNode& root() const noexcept;

    ///
    /// @brief create a DataNode corresponding to the path and set its value
    ///
    /// This methods creates a DataNode tree based on the path passed in. The path
    /// expression must identify a single node. If the last node created is of schema
    /// type list, leaf-list or anyxml that value is also set in the node.
    /// The returned DataNode is the last node created (the terminal part of the path).
    /// The user is responsible for managing the memory of this returned tree. Use DataNode#root()
    /// to get the root element of the this tree and use that pointer to dispose of the entire tree.
    /// Note in the case of List nodes the keys must be present in the path expression in the form
    /// of predicates.
    ///
    /// @param[in] path The XPath expression identifying the node relative to the root
    /// of the schema tree
    /// @param[in] value The string representation of the value to set.
    /// @return Pointer to DataNode created.
    /// @throws YCPPInvalidArgumentError In case the argument is invalid.
    /// @throws YCPPPathError In case the path is invalid.
    ///
    virtual DataNode& create(const std::string& path, const std::string& value) = 0;

    ///
    /// @brief create a DataNode corresponding to the path and set its value
    ///
    /// This methods creates a DataNode tree based on the path passed in. The path
    /// expression must identify a single node. If the last node created is of schema
    /// type list, leaf-list or anyxml that value is also set in the node.
    /// The returned DataNode is the last node created (the terminal part of the path).
    /// The user is responsible for managing the memory of this returned tree. Use DataNode#root()
    /// to get the root element of the this tree and use that pointer to dispose of the entire tree.
    /// Note in the case of List nodes the keys must be present in the path expression in the form
    /// of predicates.
    ///
    /// @param[in] path The XPath expression identifying the node.
    /// @return DataNode created or nullptr
    /// @throws YCPPInvalidArgumentError In case the argument is invalid.
    /// @throws YCPPPathError In case the path is invalid.
    ///
    virtual DataNode& create(const std::string& path)  = 0;

    ///
    /// @brief return the Statement representing this SchemaNode
    ///
    /// Note the RootSchemaNode has no YANG statement representing it.
    /// So this method returns an empty statement.
    /// @return an empty statement
    ///
   virtual Statement statement() const;

    ///
    /// @brief return vector of keys
    ///
    /// For a root node this will always return an empty vector
    /// @return empty vector
    ///
    virtual std::vector<Statement> keys() const;

    ///
    /// @brief create an rpc instance
    ///
    /// The path expression should point to a SchemaNode that represents the Rpc
    /// @param[in] path The path to the rpc schema node
    /// @return rpc or nullptr
    /// @throws YCPPInvalidArgumentError if the argument is invalid.
    /// @throws YCPPPathError if the path is invalid
    ///
    virtual std::shared_ptr<Rpc> rpc(const std::string& path) const = 0;

};

///
/// @brief DataNode
///
/// Class represents the DataNode
class DataNode{

public:
    ///
    /// @brief The destructor.
    ///
    /// Note a DataNode represents a containment hierarchy. So
    /// invocation of the destructor will lead to the children of this
    /// node being destroyed.
   virtual ~DataNode();

    ///
    /// @brief Return the SchemaNode associated with this DataNode.
    ///
    /// Return the SchemaNode associated with this DataNode.
    /// @return SchemaNode associated with this DataNode
    ///
    virtual const SchemaNode& schema() const = 0;

    ///
    /// @brief returns the XPath expression of this Node in the NodeTree
    ///
    /// Get the path expression representing this Node in in the NodeTree.
    /// @return std::string representing the path to this Node.
    ///
    virtual std::string path() const = 0;

    ///
    /// @brief create a DataNode corresponding to the path and set its value
    ///
    /// This methods creates a DataNode tree based on the path passed in. The path
    /// expression must identify a single node. If the last node created is of schema
    /// type list, leaf-list or anyxml that value is also set in the node.
    /// The returned DataNode is the last node created (the terminal part of the path).
    /// The user is responsible for managing the memory of this returned tree. Use DataNode#root()
    /// to get the root element of the this tree and use that pointer to dispose of the entire tree.
    /// Note in the case of List nodes the keys must be present in the path expression in the form
    /// of predicates.
    ///
    /// @param[in] path The XPath expression identifying the node.
    /// @param[in] value The string representation of the value to set.
    /// @return Pointer to DataNode created.
    /// @throws YCPPInvalidArgumentError In case the argument is invalid.
    /// @throws YCPPPathError In case the path is invalid.
    ///
   virtual DataNode& create(const std::string& path);

    ///
    /// @brief create a DataNode corresponding to the path and set its value
    ///
    /// This methods creates a DataNode tree based on the path passed in. The path
    /// expression must identify a single node. If the last node created is of schema
    /// type list, leaf-list or anyxml that value is also set in the node.
    /// The returned DataNode is the last node created (the terminal part of the path).
    /// The user is responsible for managing the memory of this returned tree. Use DataNode#root()
    /// to get the root element of the this tree and use that pointer to dispose of the entire tree.
    /// Note in the case of List nodes the keys must be present in the path expression in the form
    /// of predicates.
    ///
    /// @param[in] path The XPath expression identifying the node.
    /// @return Pointer to DataNode created.
    /// @throws YCPPInvalidArgumentError In case the argument is invalid.
    /// @throws YCPPPathError In case the path is invalid.
    ///
    virtual DataNode& create(const std::string& path, const std::string& value) = 0;

    ///
    /// @brief set the value of this DataNode.
    ///
    /// Set the value of this data node.
    /// Note the DataNode should represent a leaf , leaf-list or anyxml element for this to work.
    /// The value should be the string representation of the type of according to the schema.
    /// Note this method does not validate the value being set. To validate please see the ValidationService.
    ///
    /// @param[in] value The value to set. This should be the string representation of the YANG type.
    /// @throws YCPPInvalidArgumentError if the DataNode's value cannot be set (for example it represents
    /// a container)
    virtual void set(const std::string& value) = 0;



    ///
    /// @brief get the value in this DataNode
    ///
    /// Returns a copy of the value of this DataNode.
    ///
    // @returns The string representation of the value.
    ///
    virtual std::string get() const = 0;

    ///
    /// @brief finds nodes that satisfy the given path expression.
    ///
    /// Finds nodes that satisfy the given path expression. For details
    /// about the path expression see @ref
    ///
    /// @param[in] path The path expression.
    /// @return vector of DataNodes that satisfy the path expression supplied.
    virtual std::vector<std::shared_ptr<DataNode>> find(const std::string& path) const = 0 ;


    ///
    /// @brief returns the parent of this DataNode or nullptr if None exist.
    ///
    /// Returns the parent of this DataNode or nullptr if None exist
    ///
    virtual DataNode* parent() const = 0;

    ///
    /// @brief returns the children of this DataNode
    ///
    /// Returns the children of this DataNode
    ///
    virtual std::vector<std::shared_ptr<DataNode>> children() const = 0;

    ///
    /// @brief returns the root DataNode of this tree.
    ///
    /// Returns the root of the DataNode.
    ///
    virtual const DataNode& root() const = 0;

    ///
    /// @brief Add the annotation to this datanode
    ///
    /// This method adds the annotation to this datanode
    ///
    /// @param[in] an The annotation to add to this DataNode
    /// @throws YCPPInvalidArgumentError In case the argument is invalid
    ///
    virtual void add_annotation(const Annotation& an) = 0;

    ///
    /// @brief Remove the annotation
    ///
    /// This method will remove the annotation from the given node.
    /// Note the m_val is ignored.
    ///
    /// @param[in] an The reference to the annotation.
    /// @return bool If true the annotation was found and removed. false otherwise
    ///
    virtual bool remove_annotation(const Annotation& an) = 0;


    ///
    /// @brief Get the annotations associated with this data node
    ///
    /// @return vector of annotations for this node.
    ///
    virtual std::vector<Annotation> annotations() = 0;

};

///
/// @brief Capability
///
/// Class represents the Capability. An instance of Capability is defined by
/// The module name and revision along with the set of enabled features defined
/// in this modules as well as the list of deviations which target nodes defined
/// by this module
struct Capability {
   Capability(const std::string& mod, const std::string& rev);

    Capability(const std::string& mod, const std::string& rev, const std::vector<std::string>& f,
               const std::vector<std::string>& d);

    Capability(const Capability& cap);

    Capability(Capability&& cap);

    Capability& operator=(const Capability& cap);

    Capability& operator=(Capability&& cap);


    bool operator==(const Capability& cap);

    /// The module
    std::string module;

    /// The revision
    std::string revision;

    /// List of features defined in this module that are enabled.
    std::vector<std::string> features;

    /// List of deviations that target nodes defined by this module.
    std::vector<std::string> deviations;
};

///
/// @brief interface for module provider.
///
/// This is the interface for module provider
class ModelProvider {
  public:
    enum class Format {
        YANG,
        YIN
    };

    virtual ~ModelProvider() {};



    ///
    /// @brief returns the model identified by the name and version
    ///
    /// @param[in] name of the model
    /// @param[in] version of the model
    /// @param[in] format Format of the model to download
    /// @return string containing the data of the model downloaded. If empty then the model is
    /// probably cannot be provided
    ///
    virtual std::string get_model(const std::string& name, const std::string& version, Format format) = 0;
    virtual std::string get_hostname_port()=0;


};

///
/// @brief represents the Repository of YANG models.
///
/// A instance of the Repository will be used to create a RootSchemaNode given a set of Capabilities.
/// Behind the scenes the repository is responsible for loading and parsing the YANG modules and
/// creating the SchemaNode tree. ServiceProviders are expected to use the Repository#create_root_schema
/// to generate the RootSchemaNode.
///
class Repository {
public:

    ///
    /// @brief Constructor for the Repositor.
    ///
    /// Constructor
    /// Uses the temp directory to download the yang files
    /// from the model provider
    Repository();

    ///
    /// @brief Constructor for the Repository.
    ///
    /// Constructor
    /// @param[in] search_dir The path in the filesystem where yang files can be found.
    /// @throws YCPPInvalidArgumentError if the search_dir is not a valid directory in the
    /// filesystem
    Repository(const std::string& search_dir);

    ~Repository();

    ///
    /// @brief Creates the root schema based on the capabilities passed in.
    ///
    /// Creates the root schema based on the vector of capabilities passed in.
    /// This method verifies the said capabilities and can throw Errors if
    /// a module is not found in the search directory or cannot be loaded.
    ///
    /// @param[in] capabilities vector of Capability
    /// @return pointer to the RootSchemaNode or nullptr if one could not be created.
    ///
    std::shared_ptr<RootSchemaNode> create_root_schema(const std::vector<path::Capability> & capabilities) ;

    ///
    /// @brief Adds a model provider.
    ///
    /// Adds a model provider to this Repository.
    /// If the repository does not find a model while trying to create
    /// a SchemaTree it calls on the model_provider to see if the said model
    /// can be downloaded by one of them. If that fails it tries the next
    ///
    /// @param[in] module_provider The Module Provider to add
    ///
    void add_model_provider(ModelProvider* model_provider);

    ///
    /// @brief Removes a model provider.
    ///
    /// Removes the given model provider from this Repository.
    ///
    void remove_model_provider(ModelProvider* model_provider);

    ///
    /// @brief Get model providers
    ///
    /// Gets all model providers registered with this repo.
    ///
    /// @return vector of ModelProvider's
    ///
    std::vector<ModelProvider*> get_model_providers() const;


    std::string path;
 private:
    std::vector<ModelProvider*> model_providers;
    bool using_temp_directory;
};


///
/// @brief Interface for all ServiceProvider implementations
///
/// Concretes instances of ServiceProviders are expected to extend this interface.
///
class ServiceProvider
{
public:
    ///
    /// @brief return the SchemaTree supported by this instance of the ServiceProvider
    ///
    /// @return pointer to the RootSchemaNode or nullptr if one could not be created
    ///
    virtual RootSchemaNode& get_root_schema() const = 0;


    virtual ~ServiceProvider();

    virtual EncodingFormat get_encoding() const = 0;

    ///
    /// @brief invoke the Rpc
    ///
    /// invokes or executes the given rpc and Returns a DataNode pointer
    /// if the Rpc has an output modelled in YANG.
    ///
    /// @param[in] pointer to the Rpc node
    /// @return The pointer to the DataNode representing the output.
    ///
    virtual std::shared_ptr<DataNode> invoke(Rpc& rpc) const = 0 ;

};

///
///
/// @brief An instance of the YANG schmea rpc node
///
/// Instances of this class represent a YANG rpc and are modelled as Callables.
/// The input data node tree is used to populate the input parameters to the rpc
/// if any.
/// The Callable takes as a parameter the ServiceProvider that can execute this rpc
/// as its parameter returning a pointer to a DataNode tree if output is available
///
class Rpc
{
public:

    virtual ~Rpc();

    ///
    /// @brief execute/invoke the rpc through the given service provider.
    ///
    /// @param[in] sp The Service provider
    /// @areturn pointer to the DataNode or nullptr if none exists
    ///
    virtual std::shared_ptr<DataNode> operator()(const ServiceProvider& provider) = 0;

    ///
    /// @brief get the input data tree
    ///
    ///@return pointer to the input DataNode or nullptr if the rpc does not have
    /// an input element in the schema.
    ///
    virtual DataNode& input() const = 0;

    ///
    /// @brief return the SchemaNode associated with this rpc
    ///
    /// @return pointer to the SchemaNode associated with this rpc.
    virtual SchemaNode& schema() const = 0;


};

}

}

#endif /* YDK_CORE_HPP */
