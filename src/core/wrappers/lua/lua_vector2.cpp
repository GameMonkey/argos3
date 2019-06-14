/**
 * @file <argos3/core/wrappers/lua/lua_vector2.cpp>
 *
 * @author Michael Allwright <allsey87@gmail.com>
 */

#include "lua_vector2.h"

#include <argos3/core/utility/math/angles.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/utility/math/quaternion.h>

#include <argos3/core/utility/configuration/argos_exception.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/wrappers/lua/lua_utility.h>

#include <exception>

namespace argos {

   /****************************************/
   /****************************************/

   const std::string CLuaVector2::m_strMetatableKey("__metatable_vector2");

   /****************************************/
   /****************************************/

   void CLuaVector2::RegisterMetatable(lua_State* pt_state) {
      /* create a constructor as a global function */
      lua_pushcfunction(pt_state, Create);
      lua_setglobal(pt_state, "vector2");
      /* create a metatable for vector2s */
      luaL_newmetatable(pt_state, m_strMetatableKey.c_str());     
      lua_pushvalue(pt_state, -1);
      lua_setfield(pt_state, -2, "__index");
      /* set the type */
      lua_pushinteger(pt_state,
         static_cast<lua_Integer>(CLuaUtility::EARGoSType::CVector2));
      lua_setfield(pt_state, -2, "__type");
      /* register metamethods */
      CLuaUtility::AddToTable(pt_state, "__tostring", ToString);
      CLuaUtility::AddToTable(pt_state, "__eq", Equal);
      CLuaUtility::AddToTable(pt_state, "__add", Add);
      CLuaUtility::AddToTable(pt_state, "__mul", Multiply);
      CLuaUtility::AddToTable(pt_state, "__sub", Subtract);
      CLuaUtility::AddToTable(pt_state, "__unm", UnaryMinus);
      CLuaUtility::AddToTable(pt_state, "normalize", Normalize);
      CLuaUtility::AddToTable(pt_state, "length", Length);
      CLuaUtility::AddToTable(pt_state, "dot", DotProduct);
      CLuaUtility::AddToTable(pt_state, "rotate", Rotate);
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Create(lua_State* pt_state) {
      /* create vector using default constructor */
      CVector2 cVector;
      /* parse arguments */
      switch(lua_gettop(pt_state)) {
      case 0:
         /* default values */
         break;
      case 1:
         /* copy constructor */
         FromLuaState(pt_state, 1, cVector);
         break;
      case 2:
         /* value constructor */
         if(lua_isnumber(pt_state, 1) &&
            lua_isnumber(pt_state, 2)) {
            cVector.Set(lua_tonumber(pt_state, 1),
                        lua_tonumber(pt_state, 2));
         }
         else {
            lua_pushstring(pt_state, "invalid arguments to vector2");
            return lua_error(pt_state);
         }
         break;
      default:
         lua_pushstring(pt_state, "invalid arguments to vector2");
         return lua_error(pt_state);
         break;
      }
      /* create a new vector table */
      ToLuaState(pt_state, cVector);
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::FromLuaState(lua_State* pt_state,
                                  int n_index,
                                  CVector2& c_vector) {
      if(lua_type(pt_state, n_index) != LUA_TTABLE) {
         lua_pushstring(pt_state, "vector2 not found at requested index");
         return lua_error(pt_state);
      }
      if(luaL_getmetafield(pt_state, n_index, "__type") == 0) {
         lua_pushstring(pt_state, "vector2 not found at requested index");
         return lua_error(pt_state);
      }
      else {
         /* check type */
         lua_Integer nType = lua_tointeger(pt_state, -1);
         lua_pop(pt_state, 1);
         if(nType != static_cast<lua_Integer>(CLuaUtility::EARGoSType::CVector2)) {
            lua_pushstring(pt_state, "vector2 not found at requested index");
            return lua_error(pt_state);
         }
      }
      /* copy the table to the top of the stack */
      lua_pushvalue(pt_state, n_index);
      /* push the x and y components of the table onto the stack */
      lua_getfield(pt_state, -1, "x");
      lua_getfield(pt_state, -2, "y");
      /* set the vector */
      c_vector.Set(lua_tonumber(pt_state, -2),
                   lua_tonumber(pt_state, -1));
      /* clean up the stack (pop x and y values and the copy of the table) */
      lua_pop(pt_state, 3);
      return 0;
   }

   /****************************************/
   /****************************************/

   void CLuaVector2::ToLuaState(lua_State* pt_state,
                                const CVector2& c_vector) {
      /* create a new vector table */
      lua_newtable(pt_state);
      luaL_getmetatable(pt_state, m_strMetatableKey.c_str());
      lua_setmetatable(pt_state, -2);
      /* set fields */
      ToLuaState(pt_state, -1, c_vector);
   }

   /****************************************/
   /****************************************/

   void CLuaVector2::ToLuaState(lua_State* pt_state,
                                int n_index,
                                const CVector2& c_vector) {
      /* copy the table to the top of the stack */
      lua_pushvalue(pt_state, n_index);
      /* set values */
      CLuaUtility::AddToTable(pt_state, "x", c_vector.GetX());
      CLuaUtility::AddToTable(pt_state, "y", c_vector.GetY());
      /* pop the copy of the table off the stack */
      lua_pop(pt_state, 1);
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Equal(lua_State* pt_state) {
      CVector2 cFirstVector, cSecondVector;
      /* copy the operands from the stack */
      FromLuaState(pt_state, 1, cFirstVector);
      FromLuaState(pt_state, 2, cSecondVector);
      /* push the result onto the stack and return it */
      lua_pushboolean(pt_state, cFirstVector == cSecondVector);
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Add(lua_State* pt_state) {
      CVector2 cFirstVector, cSecondVector;
      /* copy the operands from the stack */
      FromLuaState(pt_state, 1, cFirstVector);
      FromLuaState(pt_state, 2, cSecondVector);
      /* push the result onto the stack and return it */
      ToLuaState(pt_state, cFirstVector + cSecondVector);
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Multiply(lua_State* pt_state) {
      CVector2 cVector;
      Real fScalar;
      /* assume the second operand is the scalar */
      if(lua_isnumber(pt_state, 2)) {
         fScalar = lua_tonumber(pt_state, 2);
         FromLuaState(pt_state, 1, cVector);
      }
      /* assume the first operand is the scalar */
      else if(lua_isnumber(pt_state, 1)) {
         fScalar = lua_tonumber(pt_state, 1);
         FromLuaState(pt_state, 2, cVector);
      }
      /* neither operands were scalars */
      else {
         lua_pushstring(pt_state, "invalid arguments for multiplication by scalar");
         return lua_error(pt_state);
      }
      /* push the result onto the stack and return it */
      cVector *= fScalar;
      ToLuaState(pt_state, cVector);
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Subtract(lua_State* pt_state) {
      CVector2 cFirstVector, cSecondVector;
      /* copy the operands from the stack */
      FromLuaState(pt_state, 1, cFirstVector);
      FromLuaState(pt_state, 2, cSecondVector);
      /* push the result onto the stack and return it */
      ToLuaState(pt_state, cFirstVector - cSecondVector);
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::UnaryMinus(lua_State* pt_state) {
      CVector2 cVector;
      /* copy the operand from the stack */
      FromLuaState(pt_state, 1, cVector);
      /* push the result onto the stack and return it */
      ToLuaState(pt_state, -cVector);
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Normalize(lua_State* pt_state) {
      CVector2 cVector;
      /* copy the operand from the stack */
      FromLuaState(pt_state, 1, cVector);
      /* modify the operand in place and return it */
      ToLuaState(pt_state, 1, cVector.Normalize());
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Length(lua_State* pt_state) {
      CVector2 cVector;
      /* copy the operand from the stack */
      FromLuaState(pt_state, 1, cVector);
      /* push the result onto the stack and return it */
      lua_pushnumber(pt_state, cVector.Length());
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::DotProduct(lua_State* pt_state) {
      CVector2 cFirstVector, cSecondVector;
      /* copy the operands from the stack */
      FromLuaState(pt_state, 1, cFirstVector);
      FromLuaState(pt_state, 2, cSecondVector);
      /* push the result onto the stack and return it */
      lua_pushnumber(pt_state, cFirstVector.DotProduct(cSecondVector));
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::Rotate(lua_State* pt_state) {
      CVector2 cVector;
      CRadians cAngle;
      /* copy the operands from the stack */
      FromLuaState(pt_state, 1, cVector);
      if(lua_isnumber(pt_state, 2)) {
         cAngle.SetValue(lua_tonumber(pt_state, 2));
      }
      else {
         lua_pushstring(pt_state, "invalid arguments for rotation by angle");
         return lua_error(pt_state);
      }
      /* pop the second operand from the stack */
      lua_pop(pt_state, 1);
      /* rotate the vector by the angle */
      cVector.Rotate(cAngle);
      /* modify the first operand in place and return it */
      ToLuaState(pt_state, 1, cVector);
      return 1;
   }

   /****************************************/
   /****************************************/

   int CLuaVector2::ToString(lua_State* pt_state) {
      CVector2 cVector;
      /* copy the operand from the stack */
      FromLuaState(pt_state, 1, cVector);
      /* convert it to a string */
      std::ostringstream ossOutput;
      ossOutput << cVector;
      /* push the string onto the stack and return it */
      lua_pushstring(pt_state, ossOutput.str().c_str());
      return 1;
   }

   /****************************************/
   /****************************************/

}
