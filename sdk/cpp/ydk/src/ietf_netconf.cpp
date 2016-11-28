
#include <sstream>
#include <iostream>
#include "entity_util.hpp"
#include "ietf_netconf.hpp"

namespace ydk {
namespace ietf_netconf {

GetConfigRpc::Output::Output()
{
    yang_name = "output"; yang_parent_name = "get-config";
}

GetConfigRpc::Output::~Output()
{
}

bool GetConfigRpc::Output::has_data() const
{
    return false;
}

bool GetConfigRpc::Output::has_operation() const
{
    return is_set(operation);
}

std::string GetConfigRpc::Output::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "output";

    return path_buffer.str();

}

EntityPath GetConfigRpc::Output::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:get-config/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* GetConfigRpc::Output::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & GetConfigRpc::Output::get_children()
{
    return children;
}

void GetConfigRpc::Output::set_value(const std::string & value_path, std::string value)
{
}

GetConfigRpc::Source::Source()
    :
    	candidate{YType::empty, "candidate"},
	 running{YType::empty, "running"},
	 startup{YType::empty, "startup"}
{
    yang_name = "source"; yang_parent_name = "get-config";
}

GetConfigRpc::Source::~Source()
{
}

bool GetConfigRpc::Source::has_data() const
{
    return candidate.is_set
	|| running.is_set
	|| startup.is_set;
}

bool GetConfigRpc::Source::has_operation() const
{
    return is_set(operation)
	|| is_set(candidate.operation)
	|| is_set(running.operation)
	|| is_set(startup.operation);
}

std::string GetConfigRpc::Source::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "source";

    return path_buffer.str();

}

EntityPath GetConfigRpc::Source::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:get-config/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (candidate.is_set || is_set(candidate.operation)) leaf_name_data.push_back(candidate.get_name_leafdata());
    if (running.is_set || is_set(running.operation)) leaf_name_data.push_back(running.get_name_leafdata());
    if (startup.is_set || is_set(startup.operation)) leaf_name_data.push_back(startup.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* GetConfigRpc::Source::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & GetConfigRpc::Source::get_children()
{
    return children;
}

void GetConfigRpc::Source::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "candidate")
    {
        candidate = value;
    }
    if(value_path == "running")
    {
        running = value;
    }
    if(value_path == "startup")
    {
        startup = value;
    }
}

GetConfigRpc::GetConfigRpc()
    :
    	with_defaults{YType::enumeration, "with-defaults"}
    	,
    output(std::make_unique<GetConfigRpc::Output>())
	,source(std::make_unique<GetConfigRpc::Source>())
{
    output->parent = this;
    children["output"] = output.get();

    source->parent = this;
    children["source"] = source.get();

    yang_name = "get-config"; yang_parent_name = "ietf-netconf";
}

GetConfigRpc::~GetConfigRpc()
{
}

bool GetConfigRpc::has_data() const
{
    return with_defaults.is_set
	|| (output !=  nullptr && output->has_data())
	|| (source !=  nullptr && source->has_data());
}

bool GetConfigRpc::has_operation() const
{
    return is_set(operation)
	|| is_set(with_defaults.operation)
	|| (output !=  nullptr && is_set(output->operation))
	|| (source !=  nullptr && is_set(source->operation));
}

std::string GetConfigRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:get-config";

    return path_buffer.str();

}

EntityPath GetConfigRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (with_defaults.is_set || is_set(with_defaults.operation)) leaf_name_data.push_back(with_defaults.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* GetConfigRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "output")
    {
        if(output != nullptr)
        {
            children["output"] = output.get();
        }
        else
        {
            output = std::make_unique<GetConfigRpc::Output>();
            output->parent = this;
            children["output"] = output.get();
        }
        return children.at("output");
    }

    if(child_yang_name == "source")
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
        else
        {
            source = std::make_unique<GetConfigRpc::Source>();
            source->parent = this;
            children["source"] = source.get();
        }
        return children.at("source");
    }

    return nullptr;
}

std::map<std::string, Entity*> & GetConfigRpc::get_children()
{
    if(children.find("output") == children.end())
    {
        if(output != nullptr)
        {
            children["output"] = output.get();
        }
    }

    if(children.find("source") == children.end())
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
    }

    return children;
}

void GetConfigRpc::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "with-defaults")
    {
        with_defaults = value;
    }
}

std::unique_ptr<Entity> GetConfigRpc::clone_ptr()
{
    return std::make_unique<GetConfigRpc>();
}
EditConfigRpc::Target::Target()
    :
    	candidate{YType::empty, "candidate"},
	 running{YType::empty, "running"}
{
    yang_name = "target"; yang_parent_name = "edit-config";
}

EditConfigRpc::Target::~Target()
{
}

bool EditConfigRpc::Target::has_data() const
{
    return candidate.is_set
	|| running.is_set;
}

bool EditConfigRpc::Target::has_operation() const
{
    return is_set(operation)
	|| is_set(candidate.operation)
	|| is_set(running.operation);
}

std::string EditConfigRpc::Target::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "target";

    return path_buffer.str();

}

EntityPath EditConfigRpc::Target::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:edit-config/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (candidate.is_set || is_set(candidate.operation)) leaf_name_data.push_back(candidate.get_name_leafdata());
    if (running.is_set || is_set(running.operation)) leaf_name_data.push_back(running.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* EditConfigRpc::Target::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & EditConfigRpc::Target::get_children()
{
    return children;
}

void EditConfigRpc::Target::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "candidate")
    {
        candidate = value;
    }
    if(value_path == "running")
    {
        running = value;
    }
}

EditConfigRpc::EditConfigRpc()
    :
    	default_operation{YType::enumeration, "default-operation"},
	 error_option{YType::enumeration, "error-option"},
	 test_option{YType::enumeration, "test-option"},
	 url{YType::str, "url"}
    	,
    target(std::make_unique<EditConfigRpc::Target>())
{
    target->parent = this;
    children["target"] = target.get();

    yang_name = "edit-config"; yang_parent_name = "ietf-netconf";
}

EditConfigRpc::~EditConfigRpc()
{
}

bool EditConfigRpc::has_data() const
{
    return default_operation.is_set
	|| error_option.is_set
	|| test_option.is_set
	|| url.is_set
	|| (target !=  nullptr && target->has_data());
}

bool EditConfigRpc::has_operation() const
{
    return is_set(operation)
	|| is_set(default_operation.operation)
	|| is_set(error_option.operation)
	|| is_set(test_option.operation)
	|| is_set(url.operation)
	|| (target !=  nullptr && is_set(target->operation));
}

std::string EditConfigRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:edit-config";

    return path_buffer.str();

}

EntityPath EditConfigRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (default_operation.is_set || is_set(default_operation.operation)) leaf_name_data.push_back(default_operation.get_name_leafdata());
    if (error_option.is_set || is_set(error_option.operation)) leaf_name_data.push_back(error_option.get_name_leafdata());
    if (test_option.is_set || is_set(test_option.operation)) leaf_name_data.push_back(test_option.get_name_leafdata());
    if (url.is_set || is_set(url.operation)) leaf_name_data.push_back(url.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* EditConfigRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "target")
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
        else
        {
            target = std::make_unique<EditConfigRpc::Target>();
            target->parent = this;
            children["target"] = target.get();
        }
        return children.at("target");
    }

    return nullptr;
}

std::map<std::string, Entity*> & EditConfigRpc::get_children()
{
    if(children.find("target") == children.end())
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
    }

    return children;
}

void EditConfigRpc::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "default-operation")
    {
        default_operation = value;
    }
    if(value_path == "error-option")
    {
        error_option = value;
    }
    if(value_path == "test-option")
    {
        test_option = value;
    }
    if(value_path == "url")
    {
        url = value;
    }
}

std::unique_ptr<Entity> EditConfigRpc::clone_ptr()
{
    return std::make_unique<EditConfigRpc>();
}
CopyConfigRpc::Target::Target()
    :
    	candidate{YType::empty, "candidate"},
	 running{YType::empty, "running"},
	 startup{YType::empty, "startup"},
	 url{YType::str, "url"}
{
    yang_name = "target"; yang_parent_name = "copy-config";
}

CopyConfigRpc::Target::~Target()
{
}

bool CopyConfigRpc::Target::has_data() const
{
    return candidate.is_set
	|| running.is_set
	|| startup.is_set
	|| url.is_set;
}

bool CopyConfigRpc::Target::has_operation() const
{
    return is_set(operation)
	|| is_set(candidate.operation)
	|| is_set(running.operation)
	|| is_set(startup.operation)
	|| is_set(url.operation);
}

std::string CopyConfigRpc::Target::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "target";

    return path_buffer.str();

}

EntityPath CopyConfigRpc::Target::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:copy-config/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (candidate.is_set || is_set(candidate.operation)) leaf_name_data.push_back(candidate.get_name_leafdata());
    if (running.is_set || is_set(running.operation)) leaf_name_data.push_back(running.get_name_leafdata());
    if (startup.is_set || is_set(startup.operation)) leaf_name_data.push_back(startup.get_name_leafdata());
    if (url.is_set || is_set(url.operation)) leaf_name_data.push_back(url.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* CopyConfigRpc::Target::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & CopyConfigRpc::Target::get_children()
{
    return children;
}

void CopyConfigRpc::Target::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "candidate")
    {
        candidate = value;
    }
    if(value_path == "running")
    {
        running = value;
    }
    if(value_path == "startup")
    {
        startup = value;
    }
    if(value_path == "url")
    {
        url = value;
    }
}

CopyConfigRpc::Source::Source()
    :
    	candidate{YType::empty, "candidate"},
	 running{YType::empty, "running"},
	 startup{YType::empty, "startup"},
	 url{YType::str, "url"}
{
    yang_name = "source"; yang_parent_name = "copy-config";
}

CopyConfigRpc::Source::~Source()
{
}

bool CopyConfigRpc::Source::has_data() const
{
    return candidate.is_set
	|| running.is_set
	|| startup.is_set
	|| url.is_set;
}

bool CopyConfigRpc::Source::has_operation() const
{
    return is_set(operation)
	|| is_set(candidate.operation)
	|| is_set(running.operation)
	|| is_set(startup.operation)
	|| is_set(url.operation);
}

std::string CopyConfigRpc::Source::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "source";

    return path_buffer.str();

}

EntityPath CopyConfigRpc::Source::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:copy-config/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (candidate.is_set || is_set(candidate.operation)) leaf_name_data.push_back(candidate.get_name_leafdata());
    if (running.is_set || is_set(running.operation)) leaf_name_data.push_back(running.get_name_leafdata());
    if (startup.is_set || is_set(startup.operation)) leaf_name_data.push_back(startup.get_name_leafdata());
    if (url.is_set || is_set(url.operation)) leaf_name_data.push_back(url.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* CopyConfigRpc::Source::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & CopyConfigRpc::Source::get_children()
{
    return children;
}

void CopyConfigRpc::Source::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "candidate")
    {
        candidate = value;
    }
    if(value_path == "running")
    {
        running = value;
    }
    if(value_path == "startup")
    {
        startup = value;
    }
    if(value_path == "url")
    {
        url = value;
    }
}

CopyConfigRpc::CopyConfigRpc()
    :
    	with_defaults{YType::enumeration, "with-defaults"}
    	,
    source(std::make_unique<CopyConfigRpc::Source>())
	,target(std::make_unique<CopyConfigRpc::Target>())
{
    source->parent = this;
    children["source"] = source.get();

    target->parent = this;
    children["target"] = target.get();

    yang_name = "copy-config"; yang_parent_name = "ietf-netconf";
}

CopyConfigRpc::~CopyConfigRpc()
{
}

bool CopyConfigRpc::has_data() const
{
    return with_defaults.is_set
	|| (source !=  nullptr && source->has_data())
	|| (target !=  nullptr && target->has_data());
}

bool CopyConfigRpc::has_operation() const
{
    return is_set(operation)
	|| is_set(with_defaults.operation)
	|| (source !=  nullptr && is_set(source->operation))
	|| (target !=  nullptr && is_set(target->operation));
}

std::string CopyConfigRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:copy-config";

    return path_buffer.str();

}

EntityPath CopyConfigRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (with_defaults.is_set || is_set(with_defaults.operation)) leaf_name_data.push_back(with_defaults.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* CopyConfigRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "source")
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
        else
        {
            source = std::make_unique<CopyConfigRpc::Source>();
            source->parent = this;
            children["source"] = source.get();
        }
        return children.at("source");
    }

    if(child_yang_name == "target")
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
        else
        {
            target = std::make_unique<CopyConfigRpc::Target>();
            target->parent = this;
            children["target"] = target.get();
        }
        return children.at("target");
    }

    return nullptr;
}

std::map<std::string, Entity*> & CopyConfigRpc::get_children()
{
    if(children.find("source") == children.end())
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
    }

    if(children.find("target") == children.end())
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
    }

    return children;
}

void CopyConfigRpc::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "with-defaults")
    {
        with_defaults = value;
    }
}

std::unique_ptr<Entity> CopyConfigRpc::clone_ptr()
{
    return std::make_unique<CopyConfigRpc>();
}
DeleteConfigRpc::Target::Target()
    :
    	startup{YType::empty, "startup"},
	 url{YType::str, "url"}
{
    yang_name = "target"; yang_parent_name = "delete-config";
}

DeleteConfigRpc::Target::~Target()
{
}

bool DeleteConfigRpc::Target::has_data() const
{
    return startup.is_set
	|| url.is_set;
}

bool DeleteConfigRpc::Target::has_operation() const
{
    return is_set(operation)
	|| is_set(startup.operation)
	|| is_set(url.operation);
}

std::string DeleteConfigRpc::Target::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "target";

    return path_buffer.str();

}

EntityPath DeleteConfigRpc::Target::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:delete-config/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (startup.is_set || is_set(startup.operation)) leaf_name_data.push_back(startup.get_name_leafdata());
    if (url.is_set || is_set(url.operation)) leaf_name_data.push_back(url.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* DeleteConfigRpc::Target::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & DeleteConfigRpc::Target::get_children()
{
    return children;
}

void DeleteConfigRpc::Target::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "startup")
    {
        startup = value;
    }
    if(value_path == "url")
    {
        url = value;
    }
}

DeleteConfigRpc::DeleteConfigRpc()
    :
    target(std::make_unique<DeleteConfigRpc::Target>())
{
    target->parent = this;
    children["target"] = target.get();

    yang_name = "delete-config"; yang_parent_name = "ietf-netconf";
}

DeleteConfigRpc::~DeleteConfigRpc()
{
}

bool DeleteConfigRpc::has_data() const
{
    return (target !=  nullptr && target->has_data());
}

bool DeleteConfigRpc::has_operation() const
{
    return is_set(operation)
	|| (target !=  nullptr && is_set(target->operation));
}

std::string DeleteConfigRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:delete-config";

    return path_buffer.str();

}

EntityPath DeleteConfigRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* DeleteConfigRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "target")
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
        else
        {
            target = std::make_unique<DeleteConfigRpc::Target>();
            target->parent = this;
            children["target"] = target.get();
        }
        return children.at("target");
    }

    return nullptr;
}

std::map<std::string, Entity*> & DeleteConfigRpc::get_children()
{
    if(children.find("target") == children.end())
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
    }

    return children;
}

void DeleteConfigRpc::set_value(const std::string & value_path, std::string value)
{
}

std::unique_ptr<Entity> DeleteConfigRpc::clone_ptr()
{
    return std::make_unique<DeleteConfigRpc>();
}
LockRpc::Target::Target()
    :
    	candidate{YType::empty, "candidate"},
	 running{YType::empty, "running"},
	 startup{YType::empty, "startup"}
{
    yang_name = "target"; yang_parent_name = "lock";
}

LockRpc::Target::~Target()
{
}

bool LockRpc::Target::has_data() const
{
    return candidate.is_set
	|| running.is_set
	|| startup.is_set;
}

bool LockRpc::Target::has_operation() const
{
    return is_set(operation)
	|| is_set(candidate.operation)
	|| is_set(running.operation)
	|| is_set(startup.operation);
}

std::string LockRpc::Target::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "target";

    return path_buffer.str();

}

EntityPath LockRpc::Target::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:lock/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (candidate.is_set || is_set(candidate.operation)) leaf_name_data.push_back(candidate.get_name_leafdata());
    if (running.is_set || is_set(running.operation)) leaf_name_data.push_back(running.get_name_leafdata());
    if (startup.is_set || is_set(startup.operation)) leaf_name_data.push_back(startup.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* LockRpc::Target::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & LockRpc::Target::get_children()
{
    return children;
}

void LockRpc::Target::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "candidate")
    {
        candidate = value;
    }
    if(value_path == "running")
    {
        running = value;
    }
    if(value_path == "startup")
    {
        startup = value;
    }
}

LockRpc::LockRpc()
    :
    target(std::make_unique<LockRpc::Target>())
{
    target->parent = this;
    children["target"] = target.get();

    yang_name = "lock"; yang_parent_name = "ietf-netconf";
}

LockRpc::~LockRpc()
{
}

bool LockRpc::has_data() const
{
    return (target !=  nullptr && target->has_data());
}

bool LockRpc::has_operation() const
{
    return is_set(operation)
	|| (target !=  nullptr && is_set(target->operation));
}

std::string LockRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:lock";

    return path_buffer.str();

}

EntityPath LockRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* LockRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "target")
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
        else
        {
            target = std::make_unique<LockRpc::Target>();
            target->parent = this;
            children["target"] = target.get();
        }
        return children.at("target");
    }

    return nullptr;
}

std::map<std::string, Entity*> & LockRpc::get_children()
{
    if(children.find("target") == children.end())
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
    }

    return children;
}

void LockRpc::set_value(const std::string & value_path, std::string value)
{
}

std::unique_ptr<Entity> LockRpc::clone_ptr()
{
    return std::make_unique<LockRpc>();
}
UnlockRpc::Target::Target()
    :
    	candidate{YType::empty, "candidate"},
	 running{YType::empty, "running"},
	 startup{YType::empty, "startup"}
{
    yang_name = "target"; yang_parent_name = "unlock";
}

UnlockRpc::Target::~Target()
{
}

bool UnlockRpc::Target::has_data() const
{
    return candidate.is_set
	|| running.is_set
	|| startup.is_set;
}

bool UnlockRpc::Target::has_operation() const
{
    return is_set(operation)
	|| is_set(candidate.operation)
	|| is_set(running.operation)
	|| is_set(startup.operation);
}

std::string UnlockRpc::Target::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "target";

    return path_buffer.str();

}

EntityPath UnlockRpc::Target::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:unlock/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (candidate.is_set || is_set(candidate.operation)) leaf_name_data.push_back(candidate.get_name_leafdata());
    if (running.is_set || is_set(running.operation)) leaf_name_data.push_back(running.get_name_leafdata());
    if (startup.is_set || is_set(startup.operation)) leaf_name_data.push_back(startup.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* UnlockRpc::Target::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & UnlockRpc::Target::get_children()
{
    return children;
}

void UnlockRpc::Target::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "candidate")
    {
        candidate = value;
    }
    if(value_path == "running")
    {
        running = value;
    }
    if(value_path == "startup")
    {
        startup = value;
    }
}

UnlockRpc::UnlockRpc()
    :
    target(std::make_unique<UnlockRpc::Target>())
{
    target->parent = this;
    children["target"] = target.get();

    yang_name = "unlock"; yang_parent_name = "ietf-netconf";
}

UnlockRpc::~UnlockRpc()
{
}

bool UnlockRpc::has_data() const
{
    return (target !=  nullptr && target->has_data());
}

bool UnlockRpc::has_operation() const
{
    return is_set(operation)
	|| (target !=  nullptr && is_set(target->operation));
}

std::string UnlockRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:unlock";

    return path_buffer.str();

}

EntityPath UnlockRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* UnlockRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "target")
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
        else
        {
            target = std::make_unique<UnlockRpc::Target>();
            target->parent = this;
            children["target"] = target.get();
        }
        return children.at("target");
    }

    return nullptr;
}

std::map<std::string, Entity*> & UnlockRpc::get_children()
{
    if(children.find("target") == children.end())
    {
        if(target != nullptr)
        {
            children["target"] = target.get();
        }
    }

    return children;
}

void UnlockRpc::set_value(const std::string & value_path, std::string value)
{
}

std::unique_ptr<Entity> UnlockRpc::clone_ptr()
{
    return std::make_unique<UnlockRpc>();
}
GetRpc::Output::Output()
{
    yang_name = "output"; yang_parent_name = "get";
}

GetRpc::Output::~Output()
{
}

bool GetRpc::Output::has_data() const
{
    return false;
}

bool GetRpc::Output::has_operation() const
{
    return is_set(operation);
}

std::string GetRpc::Output::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "output";

    return path_buffer.str();

}

EntityPath GetRpc::Output::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:get/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* GetRpc::Output::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & GetRpc::Output::get_children()
{
    return children;
}

void GetRpc::Output::set_value(const std::string & value_path, std::string value)
{
}

GetRpc::GetRpc()
    :
    	with_defaults{YType::enumeration, "with-defaults"}
    	,
    output(std::make_unique<GetRpc::Output>())
{
    output->parent = this;
    children["output"] = output.get();

    yang_name = "get"; yang_parent_name = "ietf-netconf";
}

GetRpc::~GetRpc()
{
}

bool GetRpc::has_data() const
{
    return with_defaults.is_set
	|| (output !=  nullptr && output->has_data());
}

bool GetRpc::has_operation() const
{
    return is_set(operation)
	|| is_set(with_defaults.operation)
	|| (output !=  nullptr && is_set(output->operation));
}

std::string GetRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:get";

    return path_buffer.str();

}

EntityPath GetRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (with_defaults.is_set || is_set(with_defaults.operation)) leaf_name_data.push_back(with_defaults.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* GetRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "output")
    {
        if(output != nullptr)
        {
            children["output"] = output.get();
        }
        else
        {
            output = std::make_unique<GetRpc::Output>();
            output->parent = this;
            children["output"] = output.get();
        }
        return children.at("output");
    }

    return nullptr;
}

std::map<std::string, Entity*> & GetRpc::get_children()
{
    if(children.find("output") == children.end())
    {
        if(output != nullptr)
        {
            children["output"] = output.get();
        }
    }

    return children;
}

void GetRpc::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "with-defaults")
    {
        with_defaults = value;
    }
}

std::unique_ptr<Entity> GetRpc::clone_ptr()
{
    return std::make_unique<GetRpc>();
}
CloseSessionRpc::CloseSessionRpc()
{
    yang_name = "close-session"; yang_parent_name = "ietf-netconf";
}

CloseSessionRpc::~CloseSessionRpc()
{
}

bool CloseSessionRpc::has_data() const
{
    return false;
}

bool CloseSessionRpc::has_operation() const
{
    return is_set(operation);
}

std::string CloseSessionRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:close-session";

    return path_buffer.str();

}

EntityPath CloseSessionRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* CloseSessionRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & CloseSessionRpc::get_children()
{
    return children;
}

void CloseSessionRpc::set_value(const std::string & value_path, std::string value)
{
}

std::unique_ptr<Entity> CloseSessionRpc::clone_ptr()
{
    return std::make_unique<CloseSessionRpc>();
}
KillSessionRpc::KillSessionRpc()
    :
    	session_id{YType::uint32, "session-id"}
{
    yang_name = "kill-session"; yang_parent_name = "ietf-netconf";
}

KillSessionRpc::~KillSessionRpc()
{
}

bool KillSessionRpc::has_data() const
{
    return session_id.is_set;
}

bool KillSessionRpc::has_operation() const
{
    return is_set(operation)
	|| is_set(session_id.operation);
}

std::string KillSessionRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:kill-session";

    return path_buffer.str();

}

EntityPath KillSessionRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (session_id.is_set || is_set(session_id.operation)) leaf_name_data.push_back(session_id.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* KillSessionRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & KillSessionRpc::get_children()
{
    return children;
}

void KillSessionRpc::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "session-id")
    {
        session_id = value;
    }
}

std::unique_ptr<Entity> KillSessionRpc::clone_ptr()
{
    return std::make_unique<KillSessionRpc>();
}
CommitRpc::CommitRpc()
    :
    	confirm_timeout{YType::uint32, "confirm-timeout"},
	 confirmed{YType::empty, "confirmed"},
	 persist{YType::str, "persist"},
	 persist_id{YType::str, "persist-id"}
{
    yang_name = "commit"; yang_parent_name = "ietf-netconf";
}

CommitRpc::~CommitRpc()
{
}

bool CommitRpc::has_data() const
{
    return confirm_timeout.is_set
	|| confirmed.is_set
	|| persist.is_set
	|| persist_id.is_set;
}

bool CommitRpc::has_operation() const
{
    return is_set(operation)
	|| is_set(confirm_timeout.operation)
	|| is_set(confirmed.operation)
	|| is_set(persist.operation)
	|| is_set(persist_id.operation);
}

std::string CommitRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:commit";

    return path_buffer.str();

}

EntityPath CommitRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (confirm_timeout.is_set || is_set(confirm_timeout.operation)) leaf_name_data.push_back(confirm_timeout.get_name_leafdata());
    if (confirmed.is_set || is_set(confirmed.operation)) leaf_name_data.push_back(confirmed.get_name_leafdata());
    if (persist.is_set || is_set(persist.operation)) leaf_name_data.push_back(persist.get_name_leafdata());
    if (persist_id.is_set || is_set(persist_id.operation)) leaf_name_data.push_back(persist_id.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* CommitRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & CommitRpc::get_children()
{
    return children;
}

void CommitRpc::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "confirm-timeout")
    {
        confirm_timeout = value;
    }
    if(value_path == "confirmed")
    {
        confirmed = value;
    }
    if(value_path == "persist")
    {
        persist = value;
    }
    if(value_path == "persist-id")
    {
        persist_id = value;
    }
}

std::unique_ptr<Entity> CommitRpc::clone_ptr()
{
    return std::make_unique<CommitRpc>();
}
DiscardChangesRpc::DiscardChangesRpc()
{
    yang_name = "discard-changes"; yang_parent_name = "ietf-netconf";
}

DiscardChangesRpc::~DiscardChangesRpc()
{
}

bool DiscardChangesRpc::has_data() const
{
    return false;
}

bool DiscardChangesRpc::has_operation() const
{
    return is_set(operation);
}

std::string DiscardChangesRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:discard-changes";

    return path_buffer.str();

}

EntityPath DiscardChangesRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* DiscardChangesRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & DiscardChangesRpc::get_children()
{
    return children;
}

void DiscardChangesRpc::set_value(const std::string & value_path, std::string value)
{
}

std::unique_ptr<Entity> DiscardChangesRpc::clone_ptr()
{
    return std::make_unique<DiscardChangesRpc>();
}
CancelCommitRpc::CancelCommitRpc()
    :
    	persist_id{YType::str, "persist-id"}
{
    yang_name = "cancel-commit"; yang_parent_name = "ietf-netconf";
}

CancelCommitRpc::~CancelCommitRpc()
{
}

bool CancelCommitRpc::has_data() const
{
    return persist_id.is_set;
}

bool CancelCommitRpc::has_operation() const
{
    return is_set(operation)
	|| is_set(persist_id.operation);
}

std::string CancelCommitRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:cancel-commit";

    return path_buffer.str();

}

EntityPath CancelCommitRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (persist_id.is_set || is_set(persist_id.operation)) leaf_name_data.push_back(persist_id.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* CancelCommitRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & CancelCommitRpc::get_children()
{
    return children;
}

void CancelCommitRpc::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "persist-id")
    {
        persist_id = value;
    }
}

std::unique_ptr<Entity> CancelCommitRpc::clone_ptr()
{
    return std::make_unique<CancelCommitRpc>();
}
ValidateRpc::Source::Source()
    :
    	candidate{YType::empty, "candidate"},
	 running{YType::empty, "running"},
	 startup{YType::empty, "startup"},
	 url{YType::str, "url"}
{
    yang_name = "source"; yang_parent_name = "validate";
}

ValidateRpc::Source::~Source()
{
}

bool ValidateRpc::Source::has_data() const
{
    return candidate.is_set
	|| running.is_set
	|| startup.is_set
	|| url.is_set;
}

bool ValidateRpc::Source::has_operation() const
{
    return is_set(operation)
	|| is_set(candidate.operation)
	|| is_set(running.operation)
	|| is_set(startup.operation)
	|| is_set(url.operation);
}

std::string ValidateRpc::Source::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "source";

    return path_buffer.str();

}

EntityPath ValidateRpc::Source::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf:validate/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (candidate.is_set || is_set(candidate.operation)) leaf_name_data.push_back(candidate.get_name_leafdata());
    if (running.is_set || is_set(running.operation)) leaf_name_data.push_back(running.get_name_leafdata());
    if (startup.is_set || is_set(startup.operation)) leaf_name_data.push_back(startup.get_name_leafdata());
    if (url.is_set || is_set(url.operation)) leaf_name_data.push_back(url.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* ValidateRpc::Source::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & ValidateRpc::Source::get_children()
{
    return children;
}

void ValidateRpc::Source::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "candidate")
    {
        candidate = value;
    }
    if(value_path == "running")
    {
        running = value;
    }
    if(value_path == "startup")
    {
        startup = value;
    }
    if(value_path == "url")
    {
        url = value;
    }
}

ValidateRpc::ValidateRpc()
    :
    source(std::make_unique<ValidateRpc::Source>())
{
    source->parent = this;
    children["source"] = source.get();

    yang_name = "validate"; yang_parent_name = "ietf-netconf";
}

ValidateRpc::~ValidateRpc()
{
}

bool ValidateRpc::has_data() const
{
    return (source !=  nullptr && source->has_data());
}

bool ValidateRpc::has_operation() const
{
    return is_set(operation)
	|| (source !=  nullptr && is_set(source->operation));
}

std::string ValidateRpc::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf:validate";

    return path_buffer.str();

}

EntityPath ValidateRpc::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* ValidateRpc::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "source")
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
        else
        {
            source = std::make_unique<ValidateRpc::Source>();
            source->parent = this;
            children["source"] = source.get();
        }
        return children.at("source");
    }

    return nullptr;
}

std::map<std::string, Entity*> & ValidateRpc::get_children()
{
    if(children.find("source") == children.end())
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
    }

    return children;
}

void ValidateRpc::set_value(const std::string & value_path, std::string value)
{
}

std::unique_ptr<Entity> ValidateRpc::clone_ptr()
{
    return std::make_unique<ValidateRpc>();
}

const Enum::Value ErrorSeverityTypeEnum::error {0, "error"};
const Enum::Value ErrorSeverityTypeEnum::warning {1, "warning"};

const Enum::Value ErrorTagTypeEnum::in_use {0, "in-use"};
const Enum::Value ErrorTagTypeEnum::invalid_value {1, "invalid-value"};
const Enum::Value ErrorTagTypeEnum::too_big {2, "too-big"};
const Enum::Value ErrorTagTypeEnum::missing_attribute {3, "missing-attribute"};
const Enum::Value ErrorTagTypeEnum::bad_attribute {4, "bad-attribute"};
const Enum::Value ErrorTagTypeEnum::unknown_attribute {5, "unknown-attribute"};
const Enum::Value ErrorTagTypeEnum::missing_element {6, "missing-element"};
const Enum::Value ErrorTagTypeEnum::bad_element {7, "bad-element"};
const Enum::Value ErrorTagTypeEnum::unknown_element {8, "unknown-element"};
const Enum::Value ErrorTagTypeEnum::unknown_namespace {9, "unknown-namespace"};
const Enum::Value ErrorTagTypeEnum::access_denied {10, "access-denied"};
const Enum::Value ErrorTagTypeEnum::lock_denied {11, "lock-denied"};
const Enum::Value ErrorTagTypeEnum::resource_denied {12, "resource-denied"};
const Enum::Value ErrorTagTypeEnum::rollback_failed {13, "rollback-failed"};
const Enum::Value ErrorTagTypeEnum::data_exists {14, "data-exists"};
const Enum::Value ErrorTagTypeEnum::data_missing {15, "data-missing"};
const Enum::Value ErrorTagTypeEnum::operation_not_supported {16, "operation-not-supported"};
const Enum::Value ErrorTagTypeEnum::operation_failed {17, "operation-failed"};
const Enum::Value ErrorTagTypeEnum::partial_operation {18, "partial-operation"};
const Enum::Value ErrorTagTypeEnum::malformed_message {19, "malformed-message"};

const Enum::Value EditOperationTypeEnum::merge {0, "merge"};
const Enum::Value EditOperationTypeEnum::replace {1, "replace"};
const Enum::Value EditOperationTypeEnum::create {2, "create"};
const Enum::Value EditOperationTypeEnum::delete_ {3, "delete"};
const Enum::Value EditOperationTypeEnum::remove {4, "remove"};

const Enum::Value EditConfigRpc::DefaultOperationEnum::merge {0, "merge"};
const Enum::Value EditConfigRpc::DefaultOperationEnum::replace {1, "replace"};
const Enum::Value EditConfigRpc::DefaultOperationEnum::none {2, "none"};

const Enum::Value EditConfigRpc::TestOptionEnum::test_then_set {0, "test-then-set"};
const Enum::Value EditConfigRpc::TestOptionEnum::set {1, "set"};
const Enum::Value EditConfigRpc::TestOptionEnum::test_only {2, "test-only"};

const Enum::Value EditConfigRpc::ErrorOptionEnum::stop_on_error {0, "stop-on-error"};
const Enum::Value EditConfigRpc::ErrorOptionEnum::continue_on_error {1, "continue-on-error"};
const Enum::Value EditConfigRpc::ErrorOptionEnum::rollback_on_error {2, "rollback-on-error"};


}
}

