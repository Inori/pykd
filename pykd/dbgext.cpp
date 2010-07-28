#include "stdafx.h"

#include <wdbgexts.h>

#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "dbgext.h"
#include "dbgprint.h"
#include "dbgreg.h"
#include "dbgtype.h"    
#include "dbgmodule.h"
#include "dbgsym.h"
#include "dbgmem.h"
#include "dbgsystem.h"
#include "dbgcmd.h"
#include "dbgdump.h"
#include "dbgexcept.h"
#include "dbgsession.h"

/////////////////////////////////////////////////////////////////////////////////

DbgExt    *dbgExt = NULL;

/////////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE( pykd )
{
    boost::python::def( "createSession", &dbgCreateSession );
    boost::python::def( "isSessionStart", &dbgIsSessionStart );
    boost::python::def( "symbolsPath", &dbgSymPath );
    boost::python::def( "dprint", &DbgPrint::dprint );
    boost::python::def( "dprintln", &DbgPrint::dprintln );
    boost::python::def( "loadDump", &dbgLoadDump );
    boost::python::def( "dbgCommand", &dbgCommand );
    boost::python::def( "is64bitSystem", is64bitSystem );
    boost::python::def( "reg", &loadRegister );
    boost::python::def( "typedVar", &loadTypedVar );
    boost::python::def( "containingRecord", &containingRecord );
    boost::python::def( "loadModule", &loadModule );
    boost::python::def( "findSymbol", &findSymbolForAddress );
    boost::python::def( "getOffset", &findAddressForSymbol );
    boost::python::def( "findModule", &findModule );
    boost::python::def( "addr64", &addr64 );
    boost::python::def( "loadBytes", &loadArray<unsigned char> );
    boost::python::def( "loadWords", &loadArray<unsigned short> );
    boost::python::def( "loadDWords", &loadArray<unsigned long> );
    boost::python::def( "loadQWords", &loadArray<unsigned __int64> );
    boost::python::def( "loadSignBytes", &loadArray<char> );
    boost::python::def( "loadSignWords", &loadArray<short> );
    boost::python::def( "loadSignDWords", &loadArray<long> );
    boost::python::def( "loadSignQWords", &loadArray<__int64> );
    boost::python::def( "loadPtrs", &loadPtrArray );
    boost::python::def( "PtrByte", &loadByPtr<unsigned char> );
    boost::python::def( "PtrSignByte", &loadByPtr<char> );
    boost::python::def( "PtrWord", &loadByPtr<unsigned short> );
    boost::python::def( "PtrSignWord", &loadByPtr<short> );
    boost::python::def( "PtrDWord", &loadByPtr<unsigned long> );
    boost::python::def( "PtrSignDWord", &loadByPtr<long> );
    boost::python::def( "PtrQWord", &loadByPtr<unsigned __int64> );
    boost::python::def( "PtrSignQWord", &loadByPtr<__int64> );
    boost::python::def( "PtrPtr", &loadPtrByPtr );    
    boost::python::def( "compareMemory", &compareMemory );
    boost::python::class_<typedVarClass>( "typedVarClass" )
        .def("getAddress", &typedVarClass::getAddress );
    boost::python::class_<dbgModuleClass>( "dbgModuleClass" )
        .def("begin", &dbgModuleClass::getBegin )
        .def("end", &dbgModuleClass::getEnd )
        .def("name", &dbgModuleClass::getName )
        .def("contain", &dbgModuleClass::contain );
}    

/////////////////////////////////////////////////////////////////////////////////

HRESULT
CALLBACK
DebugExtensionInitialize(
    OUT PULONG  Version,
    OUT PULONG  Flags )
{
    *Version = DEBUG_EXTENSION_VERSION( 1, 0 );
    *Flags = 0;
     
    PyImport_AppendInittab("pykd", initpykd ); 
       
    Py_Initialize();
    
    dbgSessionStarted = true;                
    
    return S_OK; 
}
    
    
VOID
CALLBACK
DebugExtensionUninitialize()
{
    Py_Finalize();  
}


void
SetupDebugEngine( IDebugClient4 *client, DbgExt *dbgExt  )
{
    client->QueryInterface( __uuidof(IDebugClient), (void **)&dbgExt->client );
    client->QueryInterface( __uuidof(IDebugClient4), (void **)&dbgExt->client4 );
    
    
    client->QueryInterface( __uuidof(IDebugControl), (void **)&dbgExt->control );
    
    client->QueryInterface( __uuidof(IDebugRegisters), (void **)&dbgExt->registers );
    
    client->QueryInterface( __uuidof(IDebugSymbols), (void ** )&dbgExt->symbols );
    client->QueryInterface( __uuidof(IDebugSymbols2), (void ** )&dbgExt->symbols2 );    
    client->QueryInterface( __uuidof(IDebugSymbols3), (void ** )&dbgExt->symbols3 );      
    
    client->QueryInterface( __uuidof(IDebugDataSpaces), (void **)&dbgExt->dataSpaces );
    
}
    
/////////////////////////////////////////////////////////////////////////////////    
    
HRESULT 
CALLBACK
py( PDEBUG_CLIENT4 client, PCSTR args)
{
			
    try {
    
        DbgExt      ext = { 0 };
    
        SetupDebugEngine( client, &ext );  
        dbgExt = &ext;        
        
        boost::python::object       main =  boost::python::import("__main__");

        boost::python::object       global(main.attr("__dict__"));

        boost::python::object       result;
        
        result =  boost::python::exec_file( args, global, global );
     
    }
    catch( boost::python::error_already_set const & )
    {
        // ������ � �������
        PyObject    *errtype = NULL, *errvalue = NULL, *traceback = NULL;
        
        PyErr_Fetch( &errtype, &errvalue, &traceback );
        
        if(errvalue != NULL) 
        {
            PyObject *s = PyObject_Str(errvalue);
            
            DbgPrint::dprintln( PyString_AS_STRING( s ) );

            Py_DECREF(s);
        }

        Py_XDECREF(errvalue);
        Py_XDECREF(errtype);
        Py_XDECREF(traceback);        
    }    
    catch(...)
    {           
    }     
    
    return S_OK;  
}

/////////////////////////////////////////////////////////////////////////////////  
