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

#include <set>

#include "common_utilities.hpp"
#include "entity_data_node_walker.hpp"
#include "xml_subtree_codec.hpp"

using namespace std;
namespace ydk
{
string trim(const string& str)
{
    const string whitespace = " \t\n\r";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

bool replace(string& subject, const string& search, const string& replace)
{
    size_t pos = 0;
    int replace_count = 0;
    while ((pos = subject.find(search, pos)) != string::npos)
    {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
         replace_count++;
    }
    return replace_count>0;
}

bool has_xml_escape_sequences(const string& xml)
{
    if (xml.find("&lt;") != string::npos  ||
        xml.find("&gt;") != string::npos  ||
        xml.find("&amp;") != string::npos ||
        xml.find("&quot;")!= string::npos ||
        xml.find("&#13;") != string::npos)
    {
        return true;
    }
    return false;
}

string replace_xml_escape_sequences(const string& xml)
{
    // Initialize table of conversion
    map<string, string> seqs_table;
    seqs_table[string("&lt;")]   = string("<");
    seqs_table[string("&gt;")]   = string(">");
    seqs_table[string("&amp;")]  = string("&");
    seqs_table[string("&quot;")] = string("\"");
    seqs_table[string("&#13;")]  = string("");

    string reply = xml;
    for (auto item : seqs_table)
    {
        size_t pos = 0;
        while ((pos = reply.find(item.first, pos)) != string::npos)
        {
            reply = reply.replace(pos, item.first.length(), item.second);
        }
    }
    return reply;
}

string entity_vector_to_string(vector<Entity*> & entity_list)
{
    string buf = "[";
    bool first = true;
    for (auto item : entity_list) {
    if (first)
        first = false;
    else
        buf += ", ";
        buf += item->get_segment_path();
    }
    buf += ']';
    return buf;
}

shared_ptr<Entity> get_top_entity_from_filter(Entity & filter)
{
    if(filter.parent == nullptr)
        return filter.clone_ptr();

    return get_top_entity_from_filter(*(filter.parent));
}

shared_ptr<Entity> read_datanode(Entity & filter, shared_ptr<path::DataNode> read_data_node)
{
    if (read_data_node == nullptr)
        return {};
    shared_ptr<Entity> top_entity = get_top_entity_from_filter(filter);
    get_entity_from_data_node(read_data_node.get(), top_entity);
    return top_entity;
}

string get_data_payload(Entity & entity, const ServiceProvider & provider)
{
    path::DataNode& datanode = get_data_node_from_entity(entity, provider.get_session().get_root_schema());
    const path::DataNode* dn = &datanode;
    while (dn && dn->get_parent())
        dn = dn->get_parent();

    if (dn == nullptr)
        return string{};

    YLOG_DEBUG("Encoding the subtree filter request using path API DataNode");
    path::Codec codec{};
    return codec.encode(*dn, provider.get_encoding(), true);
}

string get_xml_subtree_filter_payload(Entity & entity, const ServiceProvider & provider)
{
    XmlSubtreeCodec xml_subtree_codec{};
    YLOG_DEBUG("Encoding the subtree filter request using XML subtree codec");
    return xml_subtree_codec.encode(entity, provider.get_session().get_root_schema());
}

vector<string> get_union(vector<string> & v1, vector<string> & v2)
{
    set<string> all;
    YLOG_DEBUG("Performing union of vectors with {} & {} elements", v1.size(), v2.size());
    for(auto& s: v1)
    {
        all.insert(s);
    }
    for(auto& s: v2)
    {
        all.insert(s);
    }
    YLOG_DEBUG("Union contains {} elements", all.size());
    vector<string> v;
    for(auto& s: all)
    {
        YLOG_DEBUG("Adding unioned element: {}", s);
        v.push_back(s);
    }
    return v;
}
}
