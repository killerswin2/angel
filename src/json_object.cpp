#include "json_object.hpp"

// We'll use the generic interface for the factories as we need the engine pointer
static void json_factory_generic(asIScriptGeneric *gen)
{
    asIScriptEngine *engine = gen->GetEngine();

    // yes two-star programming
    *static_cast<json_object**>(gen->GetAddressOfReturnLocation()) = new json_object(engine);
}

static void json_add_ref_generic(asIScriptGeneric *gen)
{
    json_object *self = static_cast<json_object*>(gen->GetObject());
	self->add_ref();
}

static void json_release_generic(asIScriptGeneric *gen)
{
    json_object *self = static_cast<json_object*>(gen->GetObject());
	self->release();
}

static void json_get_ref_count_generic(asIScriptGeneric *gen)
{
    json_object *self = static_cast<json_object*>(gen->GetObject());
    *static_cast<int*>(gen->GetAddressOfReturnLocation()) = self->get_ref_count();
}

static void json_set_flag_generic(asIScriptGeneric *gen)
{
    json_object *self = static_cast<json_object*>(gen->GetObject());
    self->set_flag();
}

static void json_get_flag_generic(asIScriptGeneric *gen)
{
    json_object *self = static_cast<json_object*>(gen->GetObject());
    *static_cast<bool*>(gen->GetAddressOfReturnLocation()) = self->get_flag();
}

static void json_enum_references_generic(asIScriptGeneric *gen)
{

}

static void json_release_all_handles_generic(asIScriptGeneric *gen)
{

}

static void json_push_back_generic(asIScriptGeneric *gen)
{
    asIScriptEngine *engine = gen->GetEngine();
    json_object *self = static_cast<json_object*>(gen->GetObject());
    int *number = static_cast<int*>(gen->GetArgAddress(0));
    int refTypeId = gen->GetArgTypeId(0);
    self->push_back(number, refTypeId);
}

static void json_print_generic(asIScriptGeneric *gen)
{
    static_cast<json_object*>(gen->GetObject())->print();
}

static void json_assignment_generic(asIScriptGeneric *gen)
{
	json_object *other = static_cast<json_object*>(gen->GetArgObject(0));
	json_object *self = static_cast<json_object*>(gen->GetObject());

	*self = *other;

	gen->SetReturnObject(self);
}

json_object::json_object(asIScriptEngine *&engine)
{
    refCounter = 1;
    gcFlag = false;

    // Notify the garbage collector of this object
    engine->NotifyGarbageCollectorOfNewObject(this, engine->GetTypeInfoByName("json"));
    scriptEngine = engine;
}

int json_object::add_ref() const
{
    // Increase counter and clear flag set by GC
    gcFlag = false;
    return asAtomicInc(refCounter);
}

int json_object::release() const
{
    // Decrease the ref counter
    gcFlag = false;
    if( asAtomicDec(refCounter) == 0 )
    {
        // Delete this object as no more references to it exists
        delete this;
        return 0;
    }

    return refCounter;
}

int json_object::get_ref_count()
{
    return refCounter;
}

bool json_object::get_flag()
{
    return gcFlag;
}

void json_object::set_flag() {
    gcFlag = true;
}

void json_object::push_back(void *objectData, int refTypeId)
{
    int jsonType = scriptEngine->GetTypeInfoByName("json")->GetTypeId();
    int stringType = scriptEngine->GetTypeInfoByName("string")->GetTypeId();

    if(refTypeId == asTYPEID_INT32)
    {
        int* value = static_cast<int*>(objectData);
        object.push_back(*value);
    }
    if(refTypeId == asTYPEID_INT64)
    {
        long long* value = static_cast<long long*>(objectData);
        object.push_back(*value);
    }
    if(refTypeId == asTYPEID_DOUBLE)
    {
        double* value = static_cast<double*>(objectData);
        object.push_back(*value);
    }
    if(refTypeId == asTYPEID_FLOAT)
    {
        float* value = static_cast<float*>(objectData);
        object.push_back(*value);
    }
    if(refTypeId == jsonType)
    {
        json_object* value = static_cast<json_object*>(objectData);
        object.push_back(value->object);
    }
    if(refTypeId == stringType)
    {
        std::string* value = static_cast<std::string*>(objectData);
        object.push_back(*value);
    }

}

void json_object::print()
{
    std::cout << object.dump() << "\n";
}

json_object::~json_object() 
{

}

void register_json(asIScriptEngine *&engine)
{
    if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
		register_json_generic(engine);
	else
		register_json_native(engine);
}

void register_json_native(asIScriptEngine *&engine)
{
    int retCode;
    retCode = engine->RegisterObjectType("json", sizeof(json_object), asOBJ_REF | asOBJ_GC); assert(retCode >= 0);

    // We'll use the generic interface for the constructor as we need the engine pointer
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_FACTORY, "json@ f()", asFUNCTION(json_factory_generic), asCALL_GENERIC); assert( retCode >= 0 );

    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_ADDREF, "void f()", asMETHOD(json_object,add_ref), asCALL_THISCALL); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_RELEASE, "void f()", asMETHOD(json_object,release), asCALL_THISCALL); assert( retCode >= 0 );

    // gc stuff
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_GETREFCOUNT, "int f()", asMETHOD(json_object,get_ref_count), asCALL_THISCALL); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_SETGCFLAG, "void f()", asMETHOD(json_object,set_flag), asCALL_THISCALL); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_GETGCFLAG, "bool f()", asMETHOD(json_object,get_flag), asCALL_THISCALL); assert( retCode >= 0 );

    //methods
    retCode = engine->RegisterObjectMethod("json", "void push_back(?&in)", asMETHODPR(json_object,push_back,(void *, int),void), asCALL_THISCALL); assert(retCode >= 0);
    retCode = engine->RegisterObjectMethod("json", "void print()", asMETHOD(json_object,print), asCALL_THISCALL); assert(retCode >= 0);
    retCode = engine->RegisterObjectMethod("json", "json &opAssign(json&in)", asFUNCTION(json_assignment_generic), asCALL_GENERIC); assert( retCode >= 0 );
}

void register_json_generic(asIScriptEngine *&engine)
{
    int retCode;
    retCode = engine->RegisterObjectType("json", sizeof(json_object), asOBJ_REF | asOBJ_GC); assert(retCode >= 0);

    // We'll use the generic interface for the constructor as we need the engine pointer
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_FACTORY, "json@ f()", asFUNCTION(json_factory_generic), asCALL_GENERIC); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_ADDREF, "void f()", asFUNCTION(json_add_ref_generic), asCALL_GENERIC); assert( retCode >= 0 );
	retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_RELEASE, "void f()", asFUNCTION(json_release_generic), asCALL_GENERIC); assert( retCode >= 0 );

    // gc stuff
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_GETREFCOUNT, "int f()", asFUNCTION(json_get_ref_count_generic), asCALL_GENERIC); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_SETGCFLAG, "void f()", asFUNCTION(json_set_flag_generic), asCALL_GENERIC); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_GETGCFLAG, "bool f()", asFUNCTION(json_get_flag_generic), asCALL_GENERIC); assert( retCode >= 0 );
    retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_ENUMREFS, "void f(int&in)", asFUNCTION(json_enum_references_generic), asCALL_GENERIC); assert( retCode >= 0 );
	retCode = engine->RegisterObjectBehaviour("json", asBEHAVE_RELEASEREFS, "void f(int&in)", asFUNCTION(json_release_all_handles_generic), asCALL_GENERIC); assert( retCode >= 0 );

    retCode = engine->RegisterObjectMethod("json", "void push_back(?&in)", asFUNCTION(json_push_back_generic), asCALL_GENERIC); assert(retCode >= 0);
    retCode = engine->RegisterObjectMethod("json", "void print()", asFUNCTION(json_print_generic), asCALL_GENERIC); assert(retCode >= 0);
    retCode = engine->RegisterObjectMethod("json", "json &opAssign(json&in)", asFUNCTION(json_assignment_generic), asCALL_GENERIC); assert( retCode >= 0 );
}
