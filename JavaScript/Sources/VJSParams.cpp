/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
#include "VJavaScriptPrecompiled.h"

#if USE_V8_ENGINE
#include "V4DContext.h"
using namespace v8;
#else
#if VERSIONMAC
#include <4DJavaScriptCore/JavaScriptCore.h>
#else
#include <JavaScriptCore/JavaScript.h>
#endif
#endif
#include "VJSParams.h"
#include "VJSContext.h"
#include "VJSValue.h"

#include "VJSRuntime_file.h"

BEGIN_TOOLBOX_NAMESPACE

	
VJSParms_withContext::~VJSParms_withContext()
{
	GetContext().EndOfNativeCodeExecution();
}


void VJSParms_withContext::ProtectValue( JS4D::ValueRef inValue) const
{
#if USE_V8_ENGINE

#else
	JSValueProtect( GetContext(), inValue);
#endif
}


void VJSParms_withContext::UnprotectValue( JS4D::ValueRef inValue) const
{
#if USE_V8_ENGINE

#else
	JSValueUnprotect( GetContext(), inValue);
#endif
}



//======================================================
VJSParms_withException::VJSParms_withException( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::ExceptionRef* outException)
: VJSParms_withContext( inContext, inObject)
, fException( outException)
{
	fTaskContext = VTask::GetCurrent()->GetErrorContext( true);
	fErrorContext = fTaskContext->PushNewContext( false /* do not keep */, true /* silent */);
}

VJSParms_withException::~VJSParms_withException()
{
#if USE_V8_ENGINE

#else
	JS4D::ConvertErrorContextToException( GetContext(), fErrorContext, fException);
	xbox_assert( fErrorContext == fTaskContext->GetLastContext());
	fTaskContext->PopContext();
	fTaskContext->RecycleContext( fErrorContext);
#endif
}

void VJSParms_withException::SetException(JS4D::ExceptionRef inException) 
{
	if (testAssert(fException))
	{
		*fException = inException;
	}
}
void VJSParms_withException::SetException(const VJSException& inException) 
{
	SetException(inException.GetValue());
}

void VJSParms_withException::EvaluateScript( const VString& inScript, VValueSingle** outResult)
{
	VJSValue value( GetContext());
	JS4D::EvaluateScript( GetContext(), inScript, value, fException);
	if (outResult != NULL)
		*outResult = value.CreateVValue(fException,false);
}


void VJSParms_withReturnValue::ReturnBool( bool inValue)							
{
#if USE_V8_ENGINE
	fV8Arguments->GetReturnValue().Set(inValue);
#else
	VJSException	except;
	fReturnValue.SetBool( inValue, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnString( const XBOX::VString& inString)		
{
#if USE_V8_ENGINE
	VStringConvertBuffer	bufferTmp(inString,VTC_UTF_8);
	Local<String> newStr =
		String::NewFromUtf8(fV8Arguments->GetIsolate(),
							bufferTmp.GetCPointer(),
							v8::String::kNormalString,
                            bufferTmp.GetSize());
	fV8Arguments->GetReturnValue().Set(newStr);
#else	
	VJSException	except;
	fReturnValue.SetString( inString, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnTime( const XBOX::VTime& inTime)				
{
#if USE_V8_ENGINE
	VTime	unixStartTime;
	unixStartTime.FromUTCTime(1970,1,1,0,0,0,0);
	HandleScope scope(fV8Arguments->GetIsolate());
	Context::Scope context_scope(fV8Arguments->GetIsolate(),V4DContext::GetPersistentContext(fV8Arguments->GetIsolate()));
	double		dateMS = inTime.GetMilliseconds() - unixStartTime.GetMilliseconds();
	fV8Arguments->GetReturnValue().Set(v8::Date::New(dateMS));
#else
	VJSException	except;
	fReturnValue.SetTime( inTime, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnDuration( const XBOX::VDuration& inDuration)	
{
#if USE_V8_ENGINE
	HandleScope scope(fV8Arguments->GetIsolate());
	Context::Scope context_scope(fV8Arguments->GetIsolate(),V4DContext::GetPersistentContext(fV8Arguments->GetIsolate()));
	if (inDuration.IsNull())
	{
		fV8Arguments->GetReturnValue().SetNull();
	}
	else
	{
		fV8Arguments->GetReturnValue().Set(v8::Number::New(inDuration.ConvertToMilliseconds()));
	}
#else
	VJSException	except;
	fReturnValue.SetDuration( inDuration, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}

void VJSParms_withReturnValue::ReturnVValue( const XBOX::VValueSingle& inValue, bool simpleDate)	
{ 
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
#else
	VJSException	except;
	fReturnValue.SetVValue( inValue, &except, simpleDate);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnFile( XBOX::VFile *inFile)					
{
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
#else
	VJSException	except;
	fReturnValue.SetFile( inFile, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnFolder( XBOX::VFolder *inFolder)				
{ 
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
#else
	VJSException	except;
	fReturnValue.SetFolder( inFolder, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnFilePathAsFileOrFolder( const XBOX::VFilePath& inPath)	
{ 
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
#else
	VJSException	except;
	fReturnValue.SetFilePathAsFileOrFolder( inPath, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnJSONValue( const VJSONValue& inValue)			
{ 
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
#else
	VJSException	except;
	fReturnValue.SetJSONValue( inValue, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}
#endif
}
void VJSParms_withReturnValue::ReturnNullValue()
{
#if USE_V8_ENGINE
	fV8Arguments->GetReturnValue().SetNull();
#else
	fReturnValue.SetNull();
#endif
}

void VJSParms_withReturnValue::ReturnUndefinedValue()
{
#if USE_V8_ENGINE
	fV8Arguments->GetReturnValue().SetUndefined();
#else
	fReturnValue.SetUndefined();
#endif
}

void VJSParms_withReturnValue::ReturnValue( const VJSValue& inValue)
{
#if USE_V8_ENGINE
	Handle<Value> handle = Handle<Value>::New( fV8Arguments->GetIsolate(),inValue.fValue);
	fV8Arguments->GetReturnValue().Set(handle);

#else
	fReturnValue = inValue;
#endif
}


VJSParms_hasProperty::VJSParms_hasProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName)
: VJSParms_withContext( inContext, inObject)
, fPropertyName( inPropertyName)
, fReturnedBoolean(false)			// Default is to forward to parent.
{
#if USE_V8_ENGINE
	xbox_assert(false);
#else
#if VERSIONDEBUG
	JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
#endif
#endif
}

VJSParms_deleteProperty::VJSParms_deleteProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ExceptionRef* outException)
: VJSParms_withException( inContext, inObject, outException)
, fPropertyName( inPropertyName)
, fReturnedBoolean(true)			// Default is successful deletion.
{
#if USE_V8_ENGINE
	xbox_assert(false);
#else
#if VERSIONDEBUG
	JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
#endif
#endif
}

VJSParms_hasInstance::VJSParms_hasInstance(JS4D::ContextRef inContext, JS4D::ObjectRef inPossibleConstructor, JS4D::ValueRef inObjectToTest, JS4D::ExceptionRef *outException)
: VJSParms_withException(inContext, (JS4D::ObjectRef) inObjectToTest, outException)
, fPossibleConstructor((JS4D::ObjectRef) inPossibleConstructor)
,fObjectToTest((JS4D::ObjectRef) inObjectToTest)
{
}

VJSParms_getPropertyNames::VJSParms_getPropertyNames( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::PropertyNameAccumulatorRef inPropertyNames)
: VJSParms_withContext( inContext, inObject)
, fPropertyNames( inPropertyNames)
{
}


size_t VJSParms_withArguments::CountParams() const					
{
#if USE_V8_ENGINE
	return fV8Arguments->Length();
#else
	return fArgumentCount;
#endif
}

VJSValue VJSParms_withArguments::GetParamValue( size_t inIndex) const
{
#if USE_V8_ENGINE
    VJSValue	result(GetContext());
    if (inIndex > 0 && inIndex <= (*fV8Arguments).Length())
	{
		v8::HandleScope handle_scope((*fV8Arguments).GetIsolate());
		Context::Scope context_scope((*fV8Arguments).GetIsolate(),V4DContext::GetPersistentContext((*fV8Arguments).GetIsolate()));
	
//DebugMsg("VJSParms_withArguments::GetParamValue i=%d isol=%x\n",inIndex,(*fV8Arguments).GetIsolate());
	//Handle<Value> val = (*fV8Arguments)[inIndex-1];
//String::Utf8Value valStr((*fV8Arguments)[inIndex-1]l);
//DebugMsg("VJSParms_withArguments::GetParamValue '%s'\n",*valStr);

		JS4D::ContextRef	ctxRef = GetContext();
		xbox_assert( ctxRef == (*fV8Arguments).GetIsolate() );
		Handle<Value>	locVal = (*fV8Arguments)[inIndex-1];
// GH trouver pourquoi necessaire alor que transporter le handle suffit

        TryCatch try_catch;
        {
            Local<String> str  = locVal->ToString();
            String::Utf8Value  value2( locVal->ToString() );
            DebugMsg("VJSParms_withArguments::GetParamValue #%d -> '%s' (isExt=%d)\n",inIndex,*value2,(locVal->IsExternal() ? 1 :0));
        }
        result = VJSValue(GetContext(),locVal.val_);
	}
	return result;

#else
	if (inIndex > 0 && inIndex <= fArgumentCount)	// sc 05/11/2009 inIndex is 1 based
		return VJSValue( GetContext(), fArguments[inIndex-1]);
	else
		return VJSValue( GetContext(), NULL);
#endif
}


VValueSingle *VJSParms_withArguments::CreateParamVValue( size_t inIndex, bool simpleDate) const
{
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);//GH
#else
	if (inIndex >= 0 && inIndex <= fArgumentCount)
		return JS4D::ValueToVValue( GetContext(), fArguments[inIndex-1], fException, simpleDate);
#endif
	return NULL;
}


bool VJSParms_withArguments::GetParamObject( size_t inIndex, VJSObject& outObject) const
{
	bool okobj = false;
	
#if USE_V8_ENGINE
	if (inIndex > 0 && inIndex <= (*fV8Arguments).Length())
	{
		v8::HandleScope handle_scope((*fV8Arguments).GetIsolate());
		Context::Scope context_scope((*fV8Arguments).GetIsolate(),V4DContext::GetPersistentContext((*fV8Arguments).GetIsolate()));

		JS4D::ContextRef	ctxRef = GetContext();
		xbox_assert(ctxRef == (*fV8Arguments).GetIsolate());
		Handle<Value>	locVal = (*fV8Arguments)[inIndex - 1];

		TryCatch try_catch;
		{
			Local<String> str = locVal->ToString();
			String::Utf8Value  value2(locVal->ToString());
			DebugMsg("VJSParms_withArguments::GetParamObject #%d -> '%s' (isExt=%d)\n", inIndex, *value2, (locVal->IsExternal() ? 1 : 0));
		}
		outObject = VJSObject((*fV8Arguments).GetIsolate(), locVal.val_);
		okobj = true;
	}

	//outObject = VJSObject( (*fV8Arguments).GetIsolate(), (*fV8Arguments)[inIndex-1].val_ );
#else
	if (inIndex >= 0 && inIndex <= fArgumentCount && JSValueIsObject( GetContext(), fArguments[inIndex-1]) )
	{
		outObject.SetObjectRef( JSValueToObject( GetContext(), fArguments[inIndex-1], fException));
		okobj = true;
	}
	else
		outObject.SetObjectRef( NULL);
#endif	
	return okobj;
}


bool VJSParms_withArguments::GetParamJSONValue( size_t inIndex, VJSONValue& outValue) const
{
	bool ok;
#if USE_V8_ENGINE	
	if (inIndex > 0 && inIndex <= fV8Arguments->Length())
#else
	if (inIndex >= 0 && inIndex <= fArgumentCount)
#endif
	{
#if USE_V8_ENGINE
		for(;;)
			DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
		xbox_assert(false);//GH
		ok = false;
#else
		ok = JS4D::ValueToVJSONValue( GetContext(), fArguments[inIndex-1], outValue, fException);
#endif
	}
	else
	{
		outValue.SetUndefined();
		ok = false;
	}
	
	return ok;
}


bool VJSParms_withArguments::GetParamFunc( size_t inIndex, VJSObject& outObject, bool allowString) const
{
	bool okfunc = false;

	VJSValue valfunc(GetParamValue(inIndex));
	if (!valfunc.IsUndefined() && valfunc.IsObject())
	{
		VJSObject objfunc(GetContext());
		valfunc.GetObject(objfunc);
		if (objfunc.IsFunction())
		{
			outObject = objfunc;
			okfunc = true;
		}
	}
	/*
	if (!okfunc && allowString)
	{
		if (IsStringParam(inIndex))
		{
		}
	}
	*/

	return okfunc;
}



bool VJSParms_withArguments::GetParamArray( size_t inIndex, VJSArray& outArray) const
{
	bool okarray = false;

	if (IsArrayParam( inIndex))
    {
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );

#else
		outArray.SetObjectRef( JSValueToObject( GetContext(), fArguments[inIndex-1], fException));
#endif
		okarray=true;
    }
	else
		outArray.SetObjectRef( NULL);

    return okarray;
}


bool VJSParms_withArguments::IsNullParam( size_t inIndex) const
{
#if USE_V8_ENGINE
	bool result = (*fV8Arguments)[inIndex-1]->IsNull();
	return result;
#else
	return (inIndex >= 0 && inIndex <= fArgumentCount) ? JSValueIsNull( GetContext(), fArguments[inIndex-1]) : true;
#endif
}


bool VJSParms_withArguments::IsBooleanParam( size_t inIndex) const
{
#if USE_V8_ENGINE
	bool result = (*fV8Arguments)[inIndex-1]->IsBoolean();
	return result;
#else
	return (inIndex >= 0 && inIndex <= fArgumentCount) ? JSValueIsBoolean( GetContext(), fArguments[inIndex-1]) : false;
#endif
}


bool VJSParms_withArguments::IsNumberParam( size_t inIndex) const
{
#if USE_V8_ENGINE
	bool result = (*fV8Arguments)[inIndex-1]->IsNumber();
	return result;
#else
	return (inIndex >= 0 && inIndex <= fArgumentCount) ? JSValueIsNumber( GetContext(), fArguments[inIndex-1]) : false;
#endif
}


bool VJSParms_withArguments::IsStringParam( size_t inIndex) const
{
#if USE_V8_ENGINE
	bool result = (*fV8Arguments)[inIndex-1]->IsString();
	return result;
#else
	return (inIndex >= 0 && inIndex <= fArgumentCount) ? JSValueIsString( GetContext(), fArguments[inIndex-1]) : false;
#endif
}


bool VJSParms_withArguments::IsObjectParam( size_t inIndex) const
{
#if USE_V8_ENGINE
	bool result = (*fV8Arguments)[inIndex-1]->IsObject();
	return result;
#else
	return !IsNullParam(inIndex) && ((inIndex >= 0 && inIndex <= fArgumentCount) ? JSValueIsObject( GetContext(), fArguments[inIndex-1]) : false);
#endif
}


bool VJSParms_withArguments::IsFunctionParam( size_t inIndex) const
{
#if USE_V8_ENGINE
	bool result = (*fV8Arguments)[inIndex-1]->IsFunction();
	return result;
#else
	return IsObjectParam( inIndex) && JSObjectIsFunction( GetContext(), JSValueToObject( GetContext(), fArguments[inIndex-1], NULL));
#endif
}


bool VJSParms_withArguments::IsArrayParam( size_t inIndex) const
{
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
	return false;
#else
	return (inIndex >= 0 && inIndex <= fArgumentCount) ? JS4D::ValueIsInstanceOf( GetContext(), fArguments[inIndex-1], CVSTR( "Array"), NULL) : false;
#endif
}


bool VJSParms_withArguments::GetStringParam( size_t inIndex, VString& outString) const
{
#if USE_V8_ENGINE
    //DebugMsg("VJSParms_withArguments::GetStringParam for idx=%d\n",inIndex);
#endif
	VJSValue value(GetParamValue(inIndex));
	return value.GetString( outString, fException);
}


bool VJSParms_withArguments::GetLongParam( size_t inIndex, sLONG *outValue) const
{
	VJSValue value( GetParamValue( inIndex));
	return value.GetLong( outValue, fException);
}


bool VJSParms_withArguments::GetLong8Param( size_t inIndex, sLONG8 *outValue) const
{
	VJSValue value( GetParamValue( inIndex));
	return value.GetLong8( outValue, fException);
}


bool VJSParms_withArguments::GetULongParam( size_t inIndex, uLONG *outValue) const
{
	VJSValue value( GetParamValue( inIndex));
	return value.GetULong( outValue, fException);
}


bool VJSParms_withArguments::GetRealParam( size_t inIndex, Real *outValue) const
{
	VJSValue value( GetParamValue( inIndex));
	return value.GetReal( outValue, fException);
}


bool VJSParms_withArguments::GetBoolParam( size_t inIndex, bool *outValue) const
{
	VJSValue value( GetParamValue( inIndex));
	return value.GetBool( outValue, fException);
}


bool VJSParms_withArguments::GetBoolParam( size_t inIndex, const VString& inNameForTrue, const VString& inNameForFalse, size_t* outIndex) const
{
	bool result = false;
	
	if (CountParams() >= inIndex)
	{
		if (IsStringParam( inIndex))
		{
			if (outIndex != NULL)
				*outIndex = inIndex + 1;
			VString s;
			GetStringParam( inIndex, s);
			if (s == inNameForTrue)
				result = true;
		}
		else
		{
			if (IsBooleanParam( inIndex))
			{
				if (outIndex != NULL)
					*outIndex = inIndex + 1;
				GetBoolParam( inIndex, &result);
			}
		}
	}
	
	return result;
}


VFile* VJSParms_withArguments::RetainFileParam( size_t inIndex, bool allowPath) const
{
#if USE_V8_ENGINE
    DebugMsg("VJSParms_withArguments::RetainFileParam for idx=%d\n",inIndex);
#endif
	VFile* result = NULL;
	
	if ( CountParams() >= inIndex )
	{
#if USE_V8_ENGINE
	 	VJSValue paramValue(GetContext(),*((*fV8Arguments)[inIndex-1]));
		result = paramValue.GetFile( fException);
	xbox_assert(result);
	xbox_assert(false);
#else
		VJSValue paramValue( GetContext(), fArguments[inIndex-1]);
		result = paramValue.GetFile( fException);
#endif
		if (result != NULL)
		{
			result->Retain();
		}
		else
		{
			if (allowPath)
			{
				VURL url;
#if USE_V8_ENGINE
				xbox_assert(false);
#else
				if (paramValue.GetURL( url, fException))
				{
					VFilePath path;
					if (url.GetFilePath(path) && path.IsFile())
					{
						VString strpath;
						//paramValue.GetString(strpath);
						path.GetPosixPath(strpath);
						result = VJSPath::ResolveFileParam(GetContext(), strpath);
						//result = new VFile( path);
					}
					else
					{
						vThrowError( VE_FILE_BAD_NAME);
					}
				}
#endif
			}
				
		}
	}
	return result;
}



VFolder* VJSParms_withArguments::RetainFolderParam( size_t inIndex, bool allowPath) const
{
	VFolder* result = NULL;
	
	if (CountParams() >= inIndex)
	{
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
		xbox_assert(false);
#else
		VJSValue paramValue( GetContext(), fArguments[inIndex-1]);
		result = paramValue.GetFolder( fException);
#endif
		if (result != NULL)
		{
			result->Retain();
		}
		else
		{
			if (allowPath)
			{
				VURL url;
#if USE_V8_ENGINE
		xbox_assert(false);
#else
				if (paramValue.GetURL( url, fException))
				{
					VFilePath path;
					if (url.GetFilePath(path) && path.IsFolder())
					{
						VString strpath;
						path.GetPosixPath(strpath);
						result = VJSPath::ResolveFolderParam(GetContext(), strpath);
						//result = new VFolder(path);
					}
					else
					{
						vThrowError( VE_FILE_BAD_NAME);
					}
				}
#endif			
			}
		}
	}
	return result;
}



const UniChar *VJSParms_getProperty::GetPropertyNamePtr( VIndex *outLength) const
{
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	return NULL;
#else
	if (outLength != NULL)
		*outLength = JSStringGetLength( fPropertyName);
	return JSStringGetCharactersPtr( fPropertyName);
#endif
}

//======================================================


void VJSParms_getPropertyNames::AddPropertyName(const VString& inPropertyName)
{
#if USE_V8_ENGINE
	DebugMsg("VJSParms_getPropertyNames::AddPropertyName called for '%S'\n",&inPropertyName);
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
#else
	JSStringRef jsString = JS4D::VStringToString(inPropertyName);
	JSPropertyNameAccumulatorAddName( fPropertyNames, jsString);
	JSStringRelease( jsString);
#endif
}


void VJSParms_getPropertyNames::ReturnNumericalRange( size_t inBegin, size_t inEnd)
{
#if USE_V8_ENGINE
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );

#else
	for( size_t i = inBegin ; i != inEnd ; ++i)
	{
		size_t index = i;
		size_t length = 0;
		JSChar	temp[20];
		JSChar* current = &temp[20];
		*--current = 0;
		while(index > 0)
		{
			*--current = (JSChar) (CHAR_DIGIT_ZERO + index%10L);
			index /= 10L;
			length++;
		}

		JSStringRef jsString = JSStringCreateWithCharacters( current, length);

		JSPropertyNameAccumulatorAddName( fPropertyNames, jsString);

		JSStringRelease( jsString);
	}
#endif
}


#if USE_V8_ENGINE

VJSParms_construct::VJSParms_construct(const v8::FunctionCallbackInfo<v8::Value>& args)
: VJSParms_withArguments(args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
, fConstructedObject(args.GetIsolate(),args.Holder().val_)
{
	;
}

void VJSParms_withPropertyCallbackReturnValue::ReturnDouble( double inValue)							
{
	DebugMsg("VJSParms_withPropertyCallbackReturnValue::ReturnDouble val=%f\n",inValue);
	info->GetReturnValue().Set(inValue);
}
void VJSParms_withPropertyCallbackReturnValue::ReturnBool( bool inValue)							
{
	info->GetReturnValue().Set(inValue);
}
void VJSParms_withPropertyCallbackReturnValue::ReturnString( const XBOX::VString& inString)		
{
	//DebugMsg("VJSParms_withPropertyCallbackReturnValue::ReturnString str=<%S>\n",&inString);
	VStringConvertBuffer	bufferTmp(inString,VTC_UTF_8);
	Local<String> newStr =
		String::NewFromUtf8(info->GetIsolate(),
							bufferTmp.GetCPointer(),
							v8::String::kNormalString,
                            bufferTmp.GetSize());
	info->GetReturnValue().Set(newStr);
}
void VJSParms_withPropertyCallbackReturnValue::ReturnTime( const XBOX::VTime& inTime)				
{
	VTime	unixStartTime;
	unixStartTime.FromUTCTime(1970,1,1,0,0,0,0);
	HandleScope scope(info->GetIsolate());
	Context::Scope context_scope(info->GetIsolate(),V4DContext::GetPersistentContext(info->GetIsolate()));
	double		dateMS = inTime.GetMilliseconds() - unixStartTime.GetMilliseconds();
	info->GetReturnValue().Set(v8::Date::New(dateMS));
}
void VJSParms_withPropertyCallbackReturnValue::ReturnDuration( const XBOX::VDuration& inDuration)	
{
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
	/*VJSException	except;
	fReturnValue.SetDuration( inDuration, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}*/
}
void VJSParms_withPropertyCallbackReturnValue::ReturnVValue( const XBOX::VValueSingle& inValue)	
{ 
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
	/*VJSException	except;
	fReturnValue.SetVValue( inValue, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}*/
}
void VJSParms_withPropertyCallbackReturnValue::ReturnFile( XBOX::VFile *inFile)					
{ 
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
/*	VJSException	except;
	fReturnValue.SetFile( inFile, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}*/
}


void VJSParms_withPropertyCallbackReturnValue::ReturnFolder( XBOX::VFolder *inFolder)				
{ 
	DebugMsg("VJSParms_withPropertyCallbackReturnValue::ReturnFolder called\n");
	HandleScope handle_scope(info->GetIsolate());
    Context::Scope context_scope(info->GetIsolate(),V4DContext::GetPersistentContext(info->GetIsolate()));
	V4DContext*	v8Ctx = (V4DContext*)(info->GetIsolate()->GetData());
	JS4D::ClassDefinition*	clDef = (JS4D::ClassDefinition*)VJSFolderIterator::Class();
	Handle<Object> locFunc = v8Ctx->GetFunction(clDef->className);

	//Persistent<FunctionTemplate>*	pFT = v8Ctx->GetImplicitFunctionConstructor(clDef->className);
	//Local<Function> locFunc = (pFT->val_)->GetFunction();
	JS4DFolderIterator* folder = (inFolder == NULL) ? NULL : new JS4DFolderIterator( inFolder);
    DebugMsg("VJSParms_withPropertyCallbackReturnValue::ReturnFolder setting prdData=%x\n",folder);
	//Handle<External> extVal = External::New((void*)folder);
    Handle<Value> valuesArray[32];
	//valuesArray[0] = extVal;
    for( int idx=0; idx<32; idx++ )
    {
        valuesArray[idx] = v8::Integer::New(0);
    }
    Local<Value> locVal = locFunc->CallAsConstructor(32,valuesArray);
    if (locVal.val_)
    {
        if (locVal.val_->IsObject())
        {
		V4DContext::SetObjectPrivateData(info->GetIsolate(),locVal.val_,folder);

        Local<Object>	obj = locVal.val_->ToObject();
        Local<String>		constrName = obj->GetConstructorName();
        String::Utf8Value	utf8Name(constrName);
        DebugMsg("VJSParms_withPropertyCallbackReturnValue::ReturnFolder constrName=%s isFunc=%d\n",*utf8Name,locVal.val_->IsFunction());
        }
    }
    else
    {
        DebugMsg("VJSParms_withPropertyCallbackReturnValue::ReturnFolder NULL obj\n");
    }
    info->GetReturnValue().Set(locVal);
    //info->GetReturnValue().Set(String::New("ABCDEFGHI"));
    //	V4DContext::SetObjectPrivateData(inContext,ioValue,folder);
/*	VJSException	except;
	fReturnValue.SetFolder( inFolder, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}*/
}
void VJSParms_withPropertyCallbackReturnValue::ReturnFilePathAsFileOrFolder( const XBOX::VFilePath& inPath)	
{ 
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
/*	VJSException	except;
	fReturnValue.SetFilePathAsFileOrFolder( inPath, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}*/
}
void VJSParms_withPropertyCallbackReturnValue::ReturnJSONValue( const VJSONValue& inValue)			
{ 
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
/*	VJSException	except;
	fReturnValue.SetJSONValue( inValue, &except);
	if (fException)
	{
		*fException = except.GetValue();
	}*/
}

void VJSParms_withPropertyCallbackReturnValue::ReturnValue( const VJSValue& inValue)
{ 
	Handle<Value> handle = Handle<Value>::New( info->GetIsolate(),inValue.fValue);
	info->GetReturnValue().Set(handle);
}

void VJSParms_withPropertyCallbackReturnValue::ReturnNullValue()									
{
	info->GetReturnValue().SetNull();
}
void VJSParms_withPropertyCallbackReturnValue::ReturnUndefinedValue()
{ 
	info->GetReturnValue().SetUndefined();
}

const VJSValue&	VJSParms_withPropertyCallbackReturnValue::GetReturnValue() const
{
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	xbox_assert(false);
	//return fReturnValue;
}

void VJSParms_withReturnValue::ReturnDouble( const double& inValue)
{
	fV8Arguments->GetReturnValue().Set(inValue);
}


void VJSParms_callStaticFunction::ReturnRawValue( void* inData )
{
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	VJSObject	vjsObj(fThis);
	vjsObj.SetPrivateData(inData);
}

void VJSParms_callStaticFunction::ReturnThis()
{
	VJSValue	val(fThis);
	ReturnValue(val);
}

void VJSParms_callAsFunction::ReturnThis()
{
	VJSValue	val(fThis);
	ReturnValue(val);
}

VJSParms_withPropertyCallbackReturnValue::VJSParms_withPropertyCallbackReturnValue(const v8::PropertyCallbackInfo<v8::Value>* info)
:VJSParms_withException(info->GetIsolate(),NULL,NULL)
,info(info)
//,fReturnValue(info->GetIsolate())
 {
	;
 }
 
 VJSParms_withReturnValue::VJSParms_withReturnValue( const v8::FunctionCallbackInfo<v8::Value>& args ) 
: fV8Arguments(&args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
//, fReturnValue(args.GetIsolate())
 {
	 ;
 }
 
VJSParms_withReturnValue::VJSParms_withReturnValue( const v8::FunctionCallbackInfo<v8::Value>& args,v8::Value* inConstructedValue  ) 
:fV8Arguments(&args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
//, fReturnValue(args.GetIsolate())
 {
	 ;
 }
	
VJSParms_withArguments::VJSParms_withArguments( const v8::FunctionCallbackInfo<v8::Value>& args,v8::Value* inConstructedValue )
: fV8Arguments(&args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
,fConstructedObject(args.GetIsolate(),inConstructedValue)
{
	;
}

VJSParms_withArguments::VJSParms_withArguments( const v8::FunctionCallbackInfo<v8::Value>& args )
: fV8Arguments(&args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
,fConstructedObject(args.GetIsolate(),args.Holder().val_)
{
	;
}


bool VJSParms_withArguments::InternalGetParamObjectPrivateData(JS4D::ClassDefinition* inClassDef,size_t inIndex) const
{
    Local<String>		constrName = fV8Arguments->Holder()->GetConstructorName();
    String::Utf8Value	utf8Name(constrName);
    bool				isOK = true;
    if (strncmp(inClassDef->className,*utf8Name,::strlen(inClassDef->className)))
    {
        DebugMsg("VJSParms_withArguments::InternalGetParamObjectPrivateData '%s' != '%s'\n",inClassDef->className,*utf8Name);
        isOK = false;
    }
    return isOK;
}

VJSParms_callStaticFunction::VJSParms_callStaticFunction( const v8::FunctionCallbackInfo<v8::Value>& args )
: VJSParms_withArguments(args)
, VJSParms_withReturnValue(args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
, fThis(args.GetIsolate(),args.Holder().val_)
{
	;
}
		
VJSParms_callAsFunction::VJSParms_callAsFunction( const v8::FunctionCallbackInfo<v8::Value>& args, v8::Value* inConstructedValue ) 
: VJSParms_withArguments(args,inConstructedValue)
, VJSParms_withReturnValue(args,inConstructedValue)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
, fFunction (args.GetIsolate(),NULL)    //TBC
, fThis(args.GetIsolate(), NULL)
{ 
				;
}

VJSParms_callAsFunction::VJSParms_callAsFunction( const v8::FunctionCallbackInfo<v8::Value>& args ) 
: VJSParms_withArguments(args)
, VJSParms_withReturnValue(args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
, fFunction(args.GetIsolate(), args.Holder().val_)    //TBC
, fThis(args.GetIsolate(), NULL)
{
	xbox::DebugMsg("VJSParms_callAsFunction::VJSParms_callAsFunction Holder is callable=%d\n", args.Holder().val_->IsCallable());
	if (args.Holder().val_->IsCallable())
	{
		int a = 1;
	}
}


VJSParms_callAsConstructor::VJSParms_callAsConstructor(const v8::FunctionCallbackInfo<v8::Value>& args )
: VJSParms_withArguments(args)
, VJSParms_withException(args.GetIsolate(),NULL,NULL)
, fConstructedObject(args.GetIsolate(),args.Holder().val_)
{
    if (args.Holder().val_)
    {
        if (args.Holder().val_->IsObject())
        {
            Local<Object>	obj = args.Holder().val_->ToObject();
            Local<String>		constrName = obj->GetConstructorName();
            String::Utf8Value	utf8Name(constrName);
            xbox::DebugMsg(":VJSParms_callAsConstructor:VJSParms_callAsConstructor called  nbArgs=%d isConstruct=%d const=%s\n",
                args.Length(),
                (args.IsConstructCall() ? 1 : 0),
                *utf8Name)	;
        }
    }
    xbox::DebugMsg(":VJSParms_callAsConstructor:VJSParms_callAsConstructor called  nbArgs=%d isConstruct=%d \n",
        args.Length(),
        (args.IsConstructCall() ? 1 : 0),
        args.Holder()->InternalFieldCount())	;
}
bool VJSParms_callAsConstructor::IsConstructCall() const
{
    return fV8Arguments->IsConstructCall();
}


VJSParms_setProperty::VJSParms_setProperty( const XBOX::VString& inPropertyName, v8::Value* inValue, const v8::PropertyCallbackInfo<v8::Value>& info)
:VJSParms_withPropertyCallbackReturnValue(&info)
,VJSParms_withException( info.GetIsolate(), NULL, NULL)
,fValue(info.GetIsolate(),inValue)
,fPropertyName( inPropertyName)
{
	DebugMsg("VJSParms_setProperty::VJSParms_setProperty called for '%S'\n",&inPropertyName);
}

VJSParms_getProperty::VJSParms_getProperty(const XBOX::VString& inPropertyName, const v8::PropertyCallbackInfo<v8::Value>& info)
:VJSParms_withPropertyCallbackReturnValue(&info)
,VJSParms_withException( info.GetIsolate(), NULL, NULL)
,fPropertyName( inPropertyName)
{
	DebugMsg("VJSParms_getProperty::VJSParms_getProperty called for '%S'\n",&inPropertyName);
}


bool VJSParms_getProperty::GetPropertyName( XBOX::VString& outPropertyName) const
{ 
	outPropertyName = fPropertyName;
	return true;
}

bool VJSParms_getProperty::GetPropertyNameAsLong( sLONG *outValue) const
{ 
	for(;;)
		DebugMsg("%s  NOT IMPL\n",__PRETTY_FUNCTION__ );
	return false;//JS4D::StringToLong( fPropertyName, outValue);
}




#else

VJSParms_withReturnValue::VJSParms_withReturnValue( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::ExceptionRef* outException)
: VJSParms_withException( inContext, inObject, outException)
, fReturnValue( inContext)
{
	;
}

VJSParms_withArguments::VJSParms_withArguments( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
: VJSParms_withException( inContext, inObject, outException)
, fArgumentCount( inArgumentCount)
, fArguments( inArguments)
{
	;
}

VJSParms_callStaticFunction::VJSParms_callStaticFunction( JS4D::ContextRef inContext, JS4D::ObjectRef inFunction, JS4D::ObjectRef inThis, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
: VJSParms_withArguments( inContext, inFunction, inArgumentCount, inArguments, outException)
, VJSParms_withReturnValue( inContext, inFunction, outException)
, VJSParms_withException( inContext, inFunction, outException)
, fThis( inContext, inThis)
{
	;
}

JS4D::ValueRef VJSParms_callStaticFunction::GetValueRefOrNull(JS4D::ContextRef inContext)
{
	return (GetReturnValue() != NULL) ? GetReturnValue() : JS4D::MakeNull(inContext);
}


VJSParms_callAsFunction::VJSParms_callAsFunction( JS4D::ContextRef inContext, JS4D::ObjectRef inFunction, JS4D::ObjectRef inThis, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
: VJSParms_withArguments( inContext, inFunction, inArgumentCount, inArguments, outException)
, VJSParms_withReturnValue( inContext, inFunction, outException)
, VJSParms_withException( inContext, inFunction, outException)
, fFunction (inContext, inFunction)
, fThis( inContext, inThis)
{
	;
}
JS4D::ValueRef VJSParms_callAsFunction::GetValueRefOrNull(JS4D::ContextRef inContext) 
{
	return (GetReturnValue() != NULL) ? GetReturnValue() : JS4D::MakeNull(inContext);
}

VJSParms_callAsConstructor::VJSParms_callAsConstructor( JS4D::ContextRef inContext, JS4D::ObjectRef inConstructor, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
: VJSParms_withArguments( inContext, inConstructor, inArgumentCount, inArguments, outException)
, VJSParms_withException( inContext, inConstructor, outException)
, fConstructedObject( inContext)
{
	;
}

VJSParms_construct::VJSParms_construct( JS4D::ContextRef inContext, JS4D::ObjectRef inConstructor, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ExceptionRef* outException)
: VJSParms_withArguments( inContext, inConstructor, inArgumentCount, inArguments, outException)
, VJSParms_withException( inContext, inConstructor, outException)
, fConstructedObject( inContext)
{
	;
}

VJSParms_getProperty::VJSParms_getProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ExceptionRef* outException)
: VJSParms_withReturnValue( inContext, inObject, outException)
, VJSParms_withException( inContext, inObject, outException)
, fPropertyName( inPropertyName)
{
#if VERSIONDEBUG
	JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
#endif
}

bool VJSParms_getProperty::GetPropertyName( XBOX::VString& outPropertyName) const
{ 
	return JS4D::StringToVString( fPropertyName, outPropertyName);
}
bool VJSParms_getProperty::GetPropertyNameAsLong( sLONG *outValue) const
{ 
	return JS4D::StringToLong( fPropertyName, outValue);
}

VJSParms_setProperty::VJSParms_setProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ValueRef inValue, JS4D::ExceptionRef* outException)
: VJSParms_withException( inContext, inObject, outException)
, fPropertyName( inPropertyName)
, fValue( inContext, inValue)
{
#if VERSIONDEBUG
	JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
#endif
}

#endif


END_TOOLBOX_NAMESPACE