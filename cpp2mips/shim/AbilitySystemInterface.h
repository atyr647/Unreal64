#pragma once
#include "AttributeSet.h"
class IAbilitySystemInterface { public: virtual ~IAbilitySystemInterface() {} virtual class UAbilitySystemComponent *GetAbilitySystemComponent() const = 0; };
