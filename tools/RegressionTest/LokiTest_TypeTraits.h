
#define DUMP_TRAIT(type, trait)\
	if(Loki::TypeTraits<type>::trait)\
		cout<<#trait<<", ";
	
#define DUMP_TRAITS(type)\
	{\
	cout<<"("<<#type<<") ";\
	DUMP_TRAIT(type, isPointer);\
	DUMP_TRAIT(type, isReference);\
	DUMP_TRAIT(type, isMemberPointer);\
	DUMP_TRAIT(type, isStdUnsignedInt);\
	DUMP_TRAIT(type, isStdSignedInt);\
	DUMP_TRAIT(type, isStdIntegral);\
	DUMP_TRAIT(type, isStdFloat);\
	DUMP_TRAIT(type, isStdArith);\
	DUMP_TRAIT(type, isStdFundamental);\
	DUMP_TRAIT(type, isUnsignedInt);\
	DUMP_TRAIT(type, isSignedInt);\
	DUMP_TRAIT(type, isIntegral);\
	DUMP_TRAIT(type, isFloat);\
	DUMP_TRAIT(type, isArith);\
	DUMP_TRAIT(type, isFundamental);\
	DUMP_TRAIT(type, isConst);\
	DUMP_TRAIT(type, isVolatile);\
	cout<<endl;\
	}