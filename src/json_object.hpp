#include <nlohmann/json.hpp>
#include <string>
#include <cassert>
#include <iostream>

#include <angelscript.h>

class json_object
{
public:
    json_object(asIScriptEngine *&engine);

    // Memory management
	int add_ref() const;
	int release() const;

    int  get_ref_count();
	bool get_flag();
    void set_flag();
    void push_back(nlohmann::json & value);
    void push_back(const nlohmann::json & value);
    void push_back(nlohmann::json&& value);
    void push_back(const nlohmann::json&& value);

protected:
    virtual ~json_object();
    mutable int refCounter;
    mutable bool gcFlag;

    nlohmann::json object;
};


//registration stuff
void register_json_native(asIScriptEngine *&engine);