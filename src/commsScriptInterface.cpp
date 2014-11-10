#include "commsScriptInterface.h"
#include "cpuShip.h"

static CommsScriptInterface* comms_script_interface = NULL;

static int setCommsMessage(lua_State* L)
{
    if (!comms_script_interface)
        return 0;
    comms_script_interface->has_message = true;
    comms_script_interface->ship->setCommsMessage(luaL_checkstring(L, 1));
    return 0;
}

static int addCommsReply(lua_State* L)
{
    if (!comms_script_interface)
        return 0;
    
    comms_script_interface->ship->addCommsReply(comms_script_interface->reply_id, luaL_checkstring(L, 1));
    if (!lua_isfunction(L, 2)) return luaL_argerror(L, 2, "2nd argument to addCommsReply should be a function");
    
    lua_setglobal(L, ("__commsReply" + string(comms_script_interface->reply_id)).c_str());
    comms_script_interface->reply_id++;
    return 0;
}
REGISTER_SCRIPT_FUNCTION(setCommsMessage);
REGISTER_SCRIPT_FUNCTION(addCommsReply);

bool CommsScriptInterface::openCommChannel(P<PlayerSpaceship> ship, P<SpaceObject> target, string script_name)
{
    comms_script_interface = this;
    reply_id = 0;
    this->ship = ship;
    this->target = target;
    
    if (scriptObject)
        scriptObject->destroy();
    scriptObject = new ScriptObject();
    scriptObject->registerObject(ship, "PlayerSpaceship", "player");
    if (P<CpuShip>(target))
        scriptObject->registerObject(target, "CpuShip", "comms_target");
    else if (P<PlayerSpaceship>(target))
        scriptObject->registerObject(target, "PlayerSpaceship", "comms_target");
    else
        scriptObject->registerObject(target, "SpaceObject", "comms_target");
    has_message = false;
    scriptObject->run(script_name);
    comms_script_interface = NULL;
    reply_id = 0;
    return has_message;
}

void CommsScriptInterface::commChannelMessage(int32_t message_id)
{
    comms_script_interface = this;
    scriptObject->callFunction("__commsReply" + string(message_id));
    comms_script_interface = NULL;
    reply_id = 0;
}
