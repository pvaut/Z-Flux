

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Dec 27 16:47:29 2012
 */
/* Compiler settings for vc90.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __vc90_h__
#define __vc90_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAngleAxis_FWD_DEFINED__
#define __IAngleAxis_FWD_DEFINED__
typedef interface IAngleAxis IAngleAxis;
#endif 	/* __IAngleAxis_FWD_DEFINED__ */


#ifndef __IVector3D_FWD_DEFINED__
#define __IVector3D_FWD_DEFINED__
typedef interface IVector3D IVector3D;
#endif 	/* __IVector3D_FWD_DEFINED__ */


#ifndef __ISensor_FWD_DEFINED__
#define __ISensor_FWD_DEFINED__
typedef interface ISensor ISensor;
#endif 	/* __ISensor_FWD_DEFINED__ */


#ifndef __IKeyboard_FWD_DEFINED__
#define __IKeyboard_FWD_DEFINED__
typedef interface IKeyboard IKeyboard;
#endif 	/* __IKeyboard_FWD_DEFINED__ */


#ifndef __ISimpleDevice_FWD_DEFINED__
#define __ISimpleDevice_FWD_DEFINED__
typedef interface ISimpleDevice ISimpleDevice;
#endif 	/* __ISimpleDevice_FWD_DEFINED__ */


#ifndef __ITDxInfo_FWD_DEFINED__
#define __ITDxInfo_FWD_DEFINED__
typedef interface ITDxInfo ITDxInfo;
#endif 	/* __ITDxInfo_FWD_DEFINED__ */


#ifndef ___ISensorEvents_FWD_DEFINED__
#define ___ISensorEvents_FWD_DEFINED__
typedef interface _ISensorEvents _ISensorEvents;
#endif 	/* ___ISensorEvents_FWD_DEFINED__ */


#ifndef ___IKeyboardEvents_FWD_DEFINED__
#define ___IKeyboardEvents_FWD_DEFINED__
typedef interface _IKeyboardEvents _IKeyboardEvents;
#endif 	/* ___IKeyboardEvents_FWD_DEFINED__ */


#ifndef ___ISimpleDeviceEvents_FWD_DEFINED__
#define ___ISimpleDeviceEvents_FWD_DEFINED__
typedef interface _ISimpleDeviceEvents _ISimpleDeviceEvents;
#endif 	/* ___ISimpleDeviceEvents_FWD_DEFINED__ */


#ifndef ____Impl__ISimpleDeviceEvents_FWD_DEFINED__
#define ____Impl__ISimpleDeviceEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class __Impl__ISimpleDeviceEvents __Impl__ISimpleDeviceEvents;
#else
typedef struct __Impl__ISimpleDeviceEvents __Impl__ISimpleDeviceEvents;
#endif /* __cplusplus */

#endif 	/* ____Impl__ISimpleDeviceEvents_FWD_DEFINED__ */


#ifndef ____Impl__ISensorEvents_FWD_DEFINED__
#define ____Impl__ISensorEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class __Impl__ISensorEvents __Impl__ISensorEvents;
#else
typedef struct __Impl__ISensorEvents __Impl__ISensorEvents;
#endif /* __cplusplus */

#endif 	/* ____Impl__ISensorEvents_FWD_DEFINED__ */


#ifndef ____Impl__IKeyboardEvents_FWD_DEFINED__
#define ____Impl__IKeyboardEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class __Impl__IKeyboardEvents __Impl__IKeyboardEvents;
#else
typedef struct __Impl__IKeyboardEvents __Impl__IKeyboardEvents;
#endif /* __cplusplus */

#endif 	/* ____Impl__IKeyboardEvents_FWD_DEFINED__ */


/* header files for imported files */
#include "docobj.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_vc90_0000_0000 */
/* [local] */ 











extern RPC_IF_HANDLE __MIDL_itf_vc90_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_vc90_0000_0000_v0_0_s_ifspec;

#ifndef __IAngleAxis_INTERFACE_DEFINED__
#define __IAngleAxis_INTERFACE_DEFINED__

/* interface IAngleAxis */
/* [object][dual][uuid] */ 


EXTERN_C const IID IID_IAngleAxis;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1ef2baff-54e9-4706-9f61-078f7134fd35")
    IAngleAxis : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_X( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Y( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Z( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Angle( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Angle( 
            /* [in] */ double pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAngleAxisVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAngleAxis * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAngleAxis * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAngleAxis * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAngleAxis * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAngleAxis * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAngleAxis * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAngleAxis * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_X )( 
            IAngleAxis * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_X )( 
            IAngleAxis * This,
            /* [in] */ double pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Y )( 
            IAngleAxis * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Y )( 
            IAngleAxis * This,
            /* [in] */ double pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Z )( 
            IAngleAxis * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Z )( 
            IAngleAxis * This,
            /* [in] */ double pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Angle )( 
            IAngleAxis * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Angle )( 
            IAngleAxis * This,
            /* [in] */ double pVal);
        
        END_INTERFACE
    } IAngleAxisVtbl;

    interface IAngleAxis
    {
        CONST_VTBL struct IAngleAxisVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAngleAxis_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAngleAxis_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAngleAxis_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAngleAxis_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAngleAxis_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAngleAxis_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAngleAxis_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAngleAxis_get_X(This,pVal)	\
    ( (This)->lpVtbl -> get_X(This,pVal) ) 

#define IAngleAxis_put_X(This,pVal)	\
    ( (This)->lpVtbl -> put_X(This,pVal) ) 

#define IAngleAxis_get_Y(This,pVal)	\
    ( (This)->lpVtbl -> get_Y(This,pVal) ) 

#define IAngleAxis_put_Y(This,pVal)	\
    ( (This)->lpVtbl -> put_Y(This,pVal) ) 

#define IAngleAxis_get_Z(This,pVal)	\
    ( (This)->lpVtbl -> get_Z(This,pVal) ) 

#define IAngleAxis_put_Z(This,pVal)	\
    ( (This)->lpVtbl -> put_Z(This,pVal) ) 

#define IAngleAxis_get_Angle(This,pVal)	\
    ( (This)->lpVtbl -> get_Angle(This,pVal) ) 

#define IAngleAxis_put_Angle(This,pVal)	\
    ( (This)->lpVtbl -> put_Angle(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAngleAxis_INTERFACE_DEFINED__ */


#ifndef __IVector3D_INTERFACE_DEFINED__
#define __IVector3D_INTERFACE_DEFINED__

/* interface IVector3D */
/* [object][dual][uuid] */ 


EXTERN_C const IID IID_IVector3D;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8c2aa71d-2b23-43f5-a6ed-4df57e9cd8d5")
    IVector3D : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_X( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Y( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Z( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Length( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Length( 
            /* [in] */ double pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVector3DVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVector3D * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVector3D * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVector3D * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVector3D * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVector3D * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVector3D * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVector3D * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_X )( 
            IVector3D * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_X )( 
            IVector3D * This,
            /* [in] */ double pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Y )( 
            IVector3D * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Y )( 
            IVector3D * This,
            /* [in] */ double pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Z )( 
            IVector3D * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Z )( 
            IVector3D * This,
            /* [in] */ double pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Length )( 
            IVector3D * This,
            /* [retval][out] */ double *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Length )( 
            IVector3D * This,
            /* [in] */ double pVal);
        
        END_INTERFACE
    } IVector3DVtbl;

    interface IVector3D
    {
        CONST_VTBL struct IVector3DVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVector3D_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVector3D_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVector3D_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVector3D_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVector3D_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVector3D_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVector3D_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVector3D_get_X(This,pVal)	\
    ( (This)->lpVtbl -> get_X(This,pVal) ) 

#define IVector3D_put_X(This,pVal)	\
    ( (This)->lpVtbl -> put_X(This,pVal) ) 

#define IVector3D_get_Y(This,pVal)	\
    ( (This)->lpVtbl -> get_Y(This,pVal) ) 

#define IVector3D_put_Y(This,pVal)	\
    ( (This)->lpVtbl -> put_Y(This,pVal) ) 

#define IVector3D_get_Z(This,pVal)	\
    ( (This)->lpVtbl -> get_Z(This,pVal) ) 

#define IVector3D_put_Z(This,pVal)	\
    ( (This)->lpVtbl -> put_Z(This,pVal) ) 

#define IVector3D_get_Length(This,pVal)	\
    ( (This)->lpVtbl -> get_Length(This,pVal) ) 

#define IVector3D_put_Length(This,pVal)	\
    ( (This)->lpVtbl -> put_Length(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVector3D_INTERFACE_DEFINED__ */


#ifndef __ISensor_INTERFACE_DEFINED__
#define __ISensor_INTERFACE_DEFINED__

/* interface ISensor */
/* [object][dual][uuid] */ 


EXTERN_C const IID IID_ISensor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f3a6775e-6fa1-4829-bf32-5b045c29078f")
    ISensor : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Translation( 
            /* [retval][out] */ IVector3D **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Rotation( 
            /* [retval][out] */ IAngleAxis **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Device( 
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Period( 
            /* [retval][out] */ double *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISensorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISensor * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISensor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISensor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISensor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISensor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISensor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISensor * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Translation )( 
            ISensor * This,
            /* [retval][out] */ IVector3D **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Rotation )( 
            ISensor * This,
            /* [retval][out] */ IAngleAxis **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Device )( 
            ISensor * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Period )( 
            ISensor * This,
            /* [retval][out] */ double *pVal);
        
        END_INTERFACE
    } ISensorVtbl;

    interface ISensor
    {
        CONST_VTBL struct ISensorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISensor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISensor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISensor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISensor_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISensor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISensor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISensor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISensor_get_Translation(This,pVal)	\
    ( (This)->lpVtbl -> get_Translation(This,pVal) ) 

#define ISensor_get_Rotation(This,pVal)	\
    ( (This)->lpVtbl -> get_Rotation(This,pVal) ) 

#define ISensor_get_Device(This,pVal)	\
    ( (This)->lpVtbl -> get_Device(This,pVal) ) 

#define ISensor_get_Period(This,pVal)	\
    ( (This)->lpVtbl -> get_Period(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISensor_INTERFACE_DEFINED__ */


#ifndef __IKeyboard_INTERFACE_DEFINED__
#define __IKeyboard_INTERFACE_DEFINED__

/* interface IKeyboard */
/* [object][dual][uuid] */ 


EXTERN_C const IID IID_IKeyboard;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d6f968e7-2993-48d7-af24-8b602d925b2c")
    IKeyboard : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Keys( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ProgrammableKeys( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetKeyLabel( 
            /* [in] */ long key,
            /* [retval][out] */ BSTR *label) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetKeyName( 
            /* [in] */ long key,
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Device( 
            /* [retval][out] */ IDispatch **pVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsKeyDown( 
            /* [in] */ long key,
            /* [retval][out] */ VARIANT_BOOL *isDown) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsKeyUp( 
            /* [in] */ long key,
            /* [retval][out] */ VARIANT_BOOL *isUp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKeyboardVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IKeyboard * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IKeyboard * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IKeyboard * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IKeyboard * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IKeyboard * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IKeyboard * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IKeyboard * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Keys )( 
            IKeyboard * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProgrammableKeys )( 
            IKeyboard * This,
            /* [retval][out] */ long *pVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyLabel )( 
            IKeyboard * This,
            /* [in] */ long key,
            /* [retval][out] */ BSTR *label);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetKeyName )( 
            IKeyboard * This,
            /* [in] */ long key,
            /* [retval][out] */ BSTR *name);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Device )( 
            IKeyboard * This,
            /* [retval][out] */ IDispatch **pVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsKeyDown )( 
            IKeyboard * This,
            /* [in] */ long key,
            /* [retval][out] */ VARIANT_BOOL *isDown);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsKeyUp )( 
            IKeyboard * This,
            /* [in] */ long key,
            /* [retval][out] */ VARIANT_BOOL *isUp);
        
        END_INTERFACE
    } IKeyboardVtbl;

    interface IKeyboard
    {
        CONST_VTBL struct IKeyboardVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKeyboard_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IKeyboard_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IKeyboard_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IKeyboard_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IKeyboard_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IKeyboard_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IKeyboard_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IKeyboard_get_Keys(This,pVal)	\
    ( (This)->lpVtbl -> get_Keys(This,pVal) ) 

#define IKeyboard_get_ProgrammableKeys(This,pVal)	\
    ( (This)->lpVtbl -> get_ProgrammableKeys(This,pVal) ) 

#define IKeyboard_GetKeyLabel(This,key,label)	\
    ( (This)->lpVtbl -> GetKeyLabel(This,key,label) ) 

#define IKeyboard_GetKeyName(This,key,name)	\
    ( (This)->lpVtbl -> GetKeyName(This,key,name) ) 

#define IKeyboard_get_Device(This,pVal)	\
    ( (This)->lpVtbl -> get_Device(This,pVal) ) 

#define IKeyboard_IsKeyDown(This,key,isDown)	\
    ( (This)->lpVtbl -> IsKeyDown(This,key,isDown) ) 

#define IKeyboard_IsKeyUp(This,key,isUp)	\
    ( (This)->lpVtbl -> IsKeyUp(This,key,isUp) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IKeyboard_INTERFACE_DEFINED__ */


#ifndef __ISimpleDevice_INTERFACE_DEFINED__
#define __ISimpleDevice_INTERFACE_DEFINED__

/* interface ISimpleDevice */
/* [object][dual][uuid] */ 


EXTERN_C const IID IID_ISimpleDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cb3bf65e-0816-482a-bb11-64af1e837812")
    ISimpleDevice : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Connect( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Sensor( 
            /* [retval][out] */ ISensor **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Keyboard( 
            /* [retval][out] */ IKeyboard **pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_IsConnected( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LoadPreferences( 
            /* [in] */ BSTR PreferencesName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISimpleDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISimpleDevice * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISimpleDevice * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISimpleDevice * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISimpleDevice * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISimpleDevice * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISimpleDevice * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISimpleDevice * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Connect )( 
            ISimpleDevice * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            ISimpleDevice * This);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Sensor )( 
            ISimpleDevice * This,
            /* [retval][out] */ ISensor **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Keyboard )( 
            ISimpleDevice * This,
            /* [retval][out] */ IKeyboard **pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            ISimpleDevice * This,
            /* [retval][out] */ long *pVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsConnected )( 
            ISimpleDevice * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LoadPreferences )( 
            ISimpleDevice * This,
            /* [in] */ BSTR PreferencesName);
        
        END_INTERFACE
    } ISimpleDeviceVtbl;

    interface ISimpleDevice
    {
        CONST_VTBL struct ISimpleDeviceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISimpleDevice_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISimpleDevice_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISimpleDevice_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISimpleDevice_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISimpleDevice_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISimpleDevice_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISimpleDevice_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISimpleDevice_Connect(This)	\
    ( (This)->lpVtbl -> Connect(This) ) 

#define ISimpleDevice_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define ISimpleDevice_get_Sensor(This,pVal)	\
    ( (This)->lpVtbl -> get_Sensor(This,pVal) ) 

#define ISimpleDevice_get_Keyboard(This,pVal)	\
    ( (This)->lpVtbl -> get_Keyboard(This,pVal) ) 

#define ISimpleDevice_get_Type(This,pVal)	\
    ( (This)->lpVtbl -> get_Type(This,pVal) ) 

#define ISimpleDevice_get_IsConnected(This,pVal)	\
    ( (This)->lpVtbl -> get_IsConnected(This,pVal) ) 

#define ISimpleDevice_LoadPreferences(This,PreferencesName)	\
    ( (This)->lpVtbl -> LoadPreferences(This,PreferencesName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISimpleDevice_INTERFACE_DEFINED__ */


#ifndef __ITDxInfo_INTERFACE_DEFINED__
#define __ITDxInfo_INTERFACE_DEFINED__

/* interface ITDxInfo */
/* [object][dual][uuid] */ 


EXTERN_C const IID IID_ITDxInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00612962-8fb6-47b2-bf98-4e8c0ff5f559")
    ITDxInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RevisionNumber( 
            /* [retval][out] */ BSTR *revision) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITDxInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITDxInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITDxInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITDxInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITDxInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITDxInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITDxInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITDxInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RevisionNumber )( 
            ITDxInfo * This,
            /* [retval][out] */ BSTR *revision);
        
        END_INTERFACE
    } ITDxInfoVtbl;

    interface ITDxInfo
    {
        CONST_VTBL struct ITDxInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITDxInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITDxInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITDxInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITDxInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITDxInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITDxInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITDxInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITDxInfo_RevisionNumber(This,revision)	\
    ( (This)->lpVtbl -> RevisionNumber(This,revision) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITDxInfo_INTERFACE_DEFINED__ */



#ifndef __Zflux_LIBRARY_DEFINED__
#define __Zflux_LIBRARY_DEFINED__

/* library Zflux */
/* [uuid][version] */ 


EXTERN_C const IID LIBID_Zflux;

#ifndef ___ISensorEvents_DISPINTERFACE_DEFINED__
#define ___ISensorEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISensorEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__ISensorEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("e6929a4a-6f41-46c6-9252-a8cc53472cb1")
    _ISensorEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISensorEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISensorEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISensorEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISensorEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISensorEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISensorEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISensorEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISensorEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISensorEventsVtbl;

    interface _ISensorEvents
    {
        CONST_VTBL struct _ISensorEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISensorEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISensorEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISensorEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISensorEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISensorEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISensorEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISensorEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISensorEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___IKeyboardEvents_DISPINTERFACE_DEFINED__
#define ___IKeyboardEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IKeyboardEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__IKeyboardEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6b6bb0a8-4491-40cf-b1a9-c15a801fe151")
    _IKeyboardEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IKeyboardEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IKeyboardEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IKeyboardEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IKeyboardEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IKeyboardEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IKeyboardEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IKeyboardEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IKeyboardEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IKeyboardEventsVtbl;

    interface _IKeyboardEvents
    {
        CONST_VTBL struct _IKeyboardEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IKeyboardEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IKeyboardEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IKeyboardEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IKeyboardEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IKeyboardEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IKeyboardEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IKeyboardEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IKeyboardEvents_DISPINTERFACE_DEFINED__ */


#ifndef ___ISimpleDeviceEvents_DISPINTERFACE_DEFINED__
#define ___ISimpleDeviceEvents_DISPINTERFACE_DEFINED__

/* dispinterface _ISimpleDeviceEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__ISimpleDeviceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("8fe3a216-e235-49a6-9136-f9d81fdadef5")
    _ISimpleDeviceEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _ISimpleDeviceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _ISimpleDeviceEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _ISimpleDeviceEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _ISimpleDeviceEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _ISimpleDeviceEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _ISimpleDeviceEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _ISimpleDeviceEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _ISimpleDeviceEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _ISimpleDeviceEventsVtbl;

    interface _ISimpleDeviceEvents
    {
        CONST_VTBL struct _ISimpleDeviceEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _ISimpleDeviceEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _ISimpleDeviceEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _ISimpleDeviceEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _ISimpleDeviceEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _ISimpleDeviceEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _ISimpleDeviceEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _ISimpleDeviceEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___ISimpleDeviceEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID___Impl__ISimpleDeviceEvents;

#ifdef __cplusplus

class DECLSPEC_UUID("5BB1322E-5E07-33DA-A587-159FF87EE376")
__Impl__ISimpleDeviceEvents;
#endif

EXTERN_C const CLSID CLSID___Impl__ISensorEvents;

#ifdef __cplusplus

class DECLSPEC_UUID("79E329DD-66A0-3299-A46D-17C76E7BD580")
__Impl__ISensorEvents;
#endif

EXTERN_C const CLSID CLSID___Impl__IKeyboardEvents;

#ifdef __cplusplus

class DECLSPEC_UUID("DFE48A04-131B-3D9F-8536-BA63D874CB8A")
__Impl__IKeyboardEvents;
#endif
#endif /* __Zflux_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


