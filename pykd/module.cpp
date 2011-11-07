#include "stdafx.h"

#include "module.h"
#include "dbgclient.h"
#include "dbgmem.h"

namespace pykd {

///////////////////////////////////////////////////////////////////////////////////

Module loadModule( const std::string  &moduleName ) {
    return g_dbgClient->loadModule( moduleName );  
};

///////////////////////////////////////////////////////////////////////////////////

Module findModule( ULONG64  offset ) {
    return g_dbgClient->findModule( offset );
}

///////////////////////////////////////////////////////////////////////////////////

Module::Module( IDebugClient4 *client, const std::string& moduleName ) : DbgObject( client )
{
    HRESULT    hres;

    m_name = moduleName;

    hres = m_symbols->GetModuleByModuleName( moduleName.c_str(), 0, NULL, &m_base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByModuleName  failed" ); 

    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
    hres = m_symbols->GetModuleParameters( 1, &m_base, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    

    m_size = moduleParam.Size;

    char imageName[0x100];

    hres = m_symbols->GetModuleNameString( 
        DEBUG_MODNAME_IMAGE,
        DEBUG_ANY_ID,
        m_base,
        imageName,
        sizeof( imageName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

    m_imageName = std::string( imageName );
}

///////////////////////////////////////////////////////////////////////////////////

Module::Module( IDebugClient4 *client, ULONG64 offset ) : DbgObject( client )
{
    HRESULT     hres;

    offset = addr64( offset );

    ULONG       moduleIndex;
    hres = m_symbols->GetModuleByOffset( offset, 0, &moduleIndex, &m_base );
    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleByOffset failed" );

    char  moduleName[0x100];

    hres = m_symbols->GetModuleNameString( 
        DEBUG_MODNAME_MODULE,
        moduleIndex,
        0,
        moduleName,
        sizeof( moduleName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

    m_name = std::string( moduleName );

    char imageName[0x100];

    hres = m_symbols->GetModuleNameString( 
        DEBUG_MODNAME_IMAGE,
        DEBUG_ANY_ID,
        m_base,
        imageName,
        sizeof( imageName ),
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugSymbol::GetModuleNameString failed" );

    m_imageName = std::string( imageName );

    DEBUG_MODULE_PARAMETERS     moduleParam = { 0 };
    hres = m_symbols->GetModuleParameters( 1, &m_base, 0, &moduleParam );
    if ( FAILED( hres ) )
         throw DbgException( "IDebugSymbol::GetModuleParameters  failed" );    

    m_size = moduleParam.Size;
}

///////////////////////////////////////////////////////////////////////////////////

std::string
Module::getPdbName()
{
    HRESULT         hres;

    IMAGEHLP_MODULEW64      moduleInfo = {};

    hres = m_advanced->GetSymbolInformation(
        DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
        m_base,
        0,
        &moduleInfo,
        sizeof(moduleInfo),
        NULL,
        NULL,
        0,
        NULL );

    if ( FAILED( hres ) )
        throw DbgException( "IDebugAdvanced2::GetSymbolInformation failed" );

    char  pdbName[ 256 ];                
    WideCharToMultiByte( CP_ACP, 0, moduleInfo.LoadedPdbName, 256, pdbName, 256, NULL, NULL );

    return std::string( pdbName );
}

///////////////////////////////////////////////////////////////////////////////////

void
Module::reloadSymbols()
{
    HRESULT     hres;

    std::string  param = "/f ";
    param += m_imageName;

    hres = m_symbols->Reload( param.c_str() );
    if ( FAILED( hres ) )
        throw DbgException("IDebugSymbols::Reload failed" );

    m_dia = pyDia::GlobalScope::loadPdb( getPdbName() );
}

///////////////////////////////////////////////////////////////////////////////////

TypeInfo
Module::getTypeByName( const std::string  &typeName )
{
    return TypeInfo( m_dia, typeName );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVar 
Module::getTypedVarByTypeName( const std::string &typeName, ULONG64 addr )
{
   return TypedVar( TypeInfo( m_dia, typeName ), addr );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVar 
Module::getTypedVarByType( const TypeInfo &typeInfo, ULONG64 addr )
{
   return TypedVar( typeInfo, addr );
}

///////////////////////////////////////////////////////////////////////////////////

TypedVar 
Module::getTypedVarByName( const std::string &symName )
{
    pyDia::SymbolPtr  typeSym = m_dia->getChildByName( symName );

    return TypedVar( TypeInfo( typeSym->getType() ), typeSym->getRva() + m_base );
}

///////////////////////////////////////////////////////////////////////////////////

//TypedVar 
//Module::getTypedVarByAddr( ULONG64 addr )
//{
//    addr = addr64(addr);
//
//    if ( addr < m_base || addr > getEnd() )
//        throw DbgException("address is out of the module space" );
//
//    ULONG   rva = (ULONG)(addr - m_base);
//
//    for( ULONG i = 0; i < m_dia->getChildCount(); i++ )
//    {
//        pyDia::SymbolPtr   typeSym = m_dia->getChildByIndex(i);
//
//        std::string  name = m_dia->getName();
//
//        if ( typeSym->getSymTag() == SymTagData && typeSym->getRva() == rva )
//        {
//            return TypedVar( TypeInfo( typeSym->getType() ), typeSym->getRva() + m_base );    
//        }
//    }
//
//    throw DbgException("failed to find type info for this offset" );
//}

///////////////////////////////////////////////////////////////////////////////////

}; // end of namespace pykd

