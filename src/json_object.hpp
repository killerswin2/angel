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
    //void EnumReferences(asIScriptEngine *engine);
	//void ReleaseAllHandles(asIScriptEngine *engine);

    void push_back(void *objectData, int refTypeId);
    
    void print();

protected:
    virtual ~json_object();
    mutable int refCounter;
    mutable bool gcFlag;

    asIScriptEngine *scriptEngine;
    nlohmann::json object;
};


//registration stuff
void register_json(asIScriptEngine *&engine);
void register_json_native(asIScriptEngine *&engine);
void register_json_generic(asIScriptEngine *&engine);