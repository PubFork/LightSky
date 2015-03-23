/* 
 * File:   scriptSerializer.h
 * Author: Miles Lacey
 *
 * Created on March 13, 2015, 9:26 PM
 */

#ifndef __LS_SCRIPT_SERIALIZER_H__
#define	__LS_SCRIPT_SERIALIZER_H__

#include "lightsky/utils/string_utils.h"

#include "lightsky/script/setup.h"

namespace ls {
namespace script {

class variable;
class functor;

class scriptSerializer {
    private:
        variableMap_t varList;
        
        functorMap_t funcList;
        
        bool initialRead(std::istream& istr, const script_base_t baseType);
        
        bool dataRead(std::istream& istr, const script_base_t baseType);
        
        void remapKeys();
        
        template <class data_t>
        bool initialWrite(
            std::ostream& ostr,
            const std::pair<data_t* const, pointer_t<data_t>>& scriptIter
        ) const;
        
    public:
        ~scriptSerializer();
        
        scriptSerializer();
        
        scriptSerializer(const scriptSerializer&) = delete;
        
        scriptSerializer(scriptSerializer&& s);
        
        scriptSerializer& operator=(const scriptSerializer&) = delete;
        
        scriptSerializer& operator=(scriptSerializer&& s);
        
        const variableMap_t& getVariableList() const;
        
        variableMap_t& getVariableList();
        
        const functorMap_t& getFunctorList() const;
        
        functorMap_t& getFunctorList();
        
        void unload();

        /**
         * @brief Load a file
         * 
         * @param filename
         * A string object containing the relative path name to a file that
         * should be loadable into memory.
         * 
         * @return true if the file was successfully loaded. False if not.
         */
        bool loadFile(const std::string& filename);

        /**
         * @brief Load a file using a c-style wide string.
         * 
         * This method merely converts the filename into a multi-byte string
         * and calls "openFile()" using the ANSI equivalent string.
         * 
         * @param filename
         * A string object containing the relative path name to a file that
         * should be loadable into memory.
         * 
         * @return true if the file was successfully loaded. False if not.
         */
        bool loadFile(const std::wstring& filename);

        /**
         * @brief Save a file
         *
         * Use this method to save data to a file, specific to the type of
         * resource used by derived classes.
         * 
         * @param filename
         * A string object containing the relative path name to a file that
         * should be saved to the computer.
         * 
         * @param inVarList
         * A constant reference to an iteratable container object which holds
         * a set of scriptable variable objects.
         * 
         * @param inFuncList
         * A constant reference to an iteratable container object which holds
         * a set of scriptable functor objects.
         * 
         * @return true if the file was successfully saved. False if not.
         */
        bool saveFile(
            const std::string& filename,
            const variableMap_t& inVarList,
            const functorMap_t& inFuncList
        ) const;

        /**
         * @brief Save a file using a c-style string of wide (UTF-8) characters
         * 
         * This method merely converts the filename into a multi-byte string
         * and calls "saveFile()" using the ANSI equivalent string.
         * 
         * @param filename
         * A string object containing the relative path name to a file that
         * should be saved to the computer.
         * 
         * @param inVarList
         * A constant reference to an iteratable container object which holds
         * a set of scriptable variable objects.
         * 
         * @param inFuncList
         * A constant reference to an iteratable container object which holds
         * a set of scriptable functor objects.
         * 
         * @return true if the file was successfully saved. False if not.
         */
        bool saveFile(
            const std::wstring& filename,
            const variableMap_t& inVarList,
            const functorMap_t& inFuncList
        ) const;
};

} // end script namespace
} // end ls namespace

#include "lightsky/script/generic/scriptSerializer_impl.h"

#endif	/* __LS_SCRIPT_SERIALIZER_H__  */
