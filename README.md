# UE5-RPG-Combat-Game
 creating a combat action RPG game
# EXTRA
- 1, hard reference :\
  A hard reference is a direct pointer to another object or asset. When object A hard-references object B, both objects are loaded into memory together. \
  pros:\
  Simplicity: Easier to work with since everything is loaded and accessible.\
  Guaranteed Availability: Ensures the referenced asset is loaded into memory when needed.\
  Fast Access: No need to check or load the referenced asset dynamically.\
  cons:\
  High Memory Usage: All referenced objects are loaded into memory, even if some are not needed immediately.\
  Longer Load Times: Increases initial loading time as all hard-referenced objects must be loaded upfront.\
  Potential for Circular Dependencies: This can lead to complex dependency chains, making debugging and optimization harder.\
  code:
  ```c++
  UPROPERTY()
  UStaticMesh* HardMesh;
  ```
- 2, soft reference: \
  A soft reference is an indirect pointer (via a TSoftObjectPtr or TSoftClassPtr) to another object or asset. It doesn't load the referenced object into memory until explicitly requested. \
  pros:\
  Reduced Memory Footprint: Assets are only loaded into memory when required, helping to optimize memory usage. \
  Faster Initial Load Times: Reduces startup loading times by deferring asset loading. \
  Flexible Asset Management: Soft references allow dynamic loading and unloading, which is ideal for modular systems. \
  cons:\
  Management Complexity: Requires additional logic to load and check the availability of assets. \
  Performance Overhead: Loading assets at runtime can introduce hitches or delays if not properly managed (e.g., using asynchronous loading). \
  Error-Prone: If not handled properly, a soft reference can lead to runtime errors (e.g., attempting to use an asset before it is fully loaded). 
  ```c++
  UPROPERTY()
  TSoftObjectPtr<UStaticMesh> SoftMesh;
  ```
  To load a soft reference: 
  ```c++
   if (SoftMesh.IsValid())
  {
      UStaticMesh* Mesh = SoftMesh.Get(); // Already loaded
  }
  else
  {
      // Asynchronous loading
      SoftMesh.ToSoftObjectPath().TryLoad();
  }
  ```
- 3, conclusion: \
  ![Screenshot 2025-01-07 100832](https://github.com/user-attachments/assets/4895bb3b-7242-45c5-a380-8f636d9ba03f)
- 4, TObjectPtr: \
  "T" in UE5, means the Type, such like TArray, TMap; \
  TObjectPtr is *hard* reference in UObject pointer; \
  ![Screenshot 2025-01-07 103243](https://github.com/user-attachments/assets/03a5a1ba-d070-4fa8-b3f8-6c3645a17bc0)
- 5, TSubclassof\
  Wrap up the required class type\
  Example:
  ```c++
  TArray< TSubclassOf < UWarriorGameplayAbility > > ActivateOnGivenAbilities;	
  ```
  We wrap up any types under `UWarriorGameplayAbility` that satisfies required of `TArray<UClass *>`

# 1, Set Up Hero Character
- 1, Base Class Structure \
  GameMode -> character -> characterController ;
- 2, DebugHelperFunction
  ```c++
  namespace Debug
  {
      static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 InKey = -1)
      {
          if (GEngine)
          {
              GEngine -> AddOnScreenDebugMessage(InKey, 7.f, Color, Msg);
  
              UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
          }
      }
  }
  ```
  - 3, Set up Camera Component
  - create springarm and camera
    ```c++
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
   	USpringArmComponent* CameraBoom;
   
   	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
   	UCameraComponent* FollowCamera;
    ```
    ```c++
    GetCapsuleComponent() -> InitCapsuleSize(42.f, 96.f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom -> SetupAttachment(GetRootComponent());
    CameraBoom -> TargetArmLength = 200.f;
    CameraBoom -> SocketOffset = FVector(0.f, 55.f, 65.f);
    CameraBoom -> bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera -> SetupAttachment(CameraBoom);
    FollowCamera->bUsePawnControlRotation = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
   	GetCharacterMovement()->RotationRate = FRotator(0.f,500.f,0.f);
   	GetCharacterMovement()->MaxWalkSpeed = 400.f;
   	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    ```
  - 4, Create a Gameplay Tags\
    WHY: In a small project, we can use traditional action binding in .h and callback. However, if in a big project, it will be complicated to handle bunches of declared actions.
  - Edit -> Project Settings -> GameplayTags
    ![Screenshot 2025-01-12 195446](https://github.com/user-attachments/assets/cb0f33fd-6ad3-4500-8227-7b127c0f6c9f)
  - create an empty C++ names WarriorGameplayTags\
    In .h\
    ```c++
    #pragma once

    #include "NativeGameplayTags.h"
    
    namespace WarriorGameplayTags
    {
    	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move)
    	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look)
    }
    ```
    In .cpp\
    ```c++
    #include "WarriorGameplayTags.h"

    namespace WarriorGameplayTags
    {
        UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,"InputTag.Move")
    	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,"InputTag.Look")
    }
    ```
    However, you will find an error on ue compiler:
    ```c++
    WarriorGameplayTags.cpp.obj : error LNK2019: unresolved external symbol "__declspec(dllimport) public: __cdecl FNativeGameplayTag::FNativeGameplayTag(class FName,class FName,class FName,class FString const &,enum ENativeGameplayTagToken)" (__imp_??0FNativeGameplayTag@@QEAA@VFName@@00AEBVFString@@W4ENativeGameplayTagToken@@@Z) referenced in function "void __cdecl WarriorGameplayTags::`dynamic initializer for 'InputTag_Look''(void)" (??__EInputTag_Look@WarriorGameplayTags@@YAXXZ)
    ```
  - fix the error\
    Go to Warrior.build.cs file:
    ```c++
    PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags" });
    ```
 - 5, Input Data config Assest\
   Create a new c++ dataasset renames DataAsset_InputConfig\
   In .h
   ```c++
   USTRUCT(BlueprintType)
   struct FWarriorInputActionConfig
   {
   	GENERATED_BODY()
   
   public:
   	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
   	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	UInputAction* InputAction;
   };
  
   UCLASS()
   class WARRIOR_API UDataAsset_InputConfig : public UDataAsset
   {
   	GENERATED_BODY()
   
   public:
   	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
   	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorInputActionConfig> NativeInputActions;

	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag);
   };
   ```
   In .cpp
   ```c++
   UInputAction *UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag &InInputTag)
   {
       for (const FWarriorInputActionConfig& InputActionConfig: NativeInputActions)
   	{
   		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
   		{
   			return InputActionConfig.InputAction;
   		}
   	}
       return nullptr;
   }
   ```
  After compiling, in character folder, goes to miscellaneous -> dataasset -> Data Asset InputConfig, rename it DA_InputConfig, \
  Then using the default third-person provided by ue's Input folders' IMC_Default, set up the DA_InputConfig:
  ![Screenshot 2025-01-12 212251](https://github.com/user-attachments/assets/fec354c2-9419-4725-8446-d431265eafbc)
- 6, Custom Enhanced Input Component\
  Create a new enhanceinputcomponent c++ under public/Components/input folder, named WarriorInputComponent\
  In DataAsset_InputConfig.h, add const for the FindNativeInputActionByTag
  ```c++
  UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const;
  ```
  In WarriorInputComponent.h, using `BindAction()` function:
  ```c++
  FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent, UObject* Object, FName FunctionName)
  ```
  For UInputAction, using FindNativeInputActionByTag() from DataAsset_InputConfig.h
  ```c++
  UInputAction* FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag)
  ```
  For ETriggerEvent, declare a new ETriggerEvent TriggerEvent by EnhancedInputComponent.h
  ```c++
  ETriggerEvent TriggerEvent
  ```
  For variable: Object, FunctionName, declare their types by template
  ```c++
  template<class UserObject, typename CallbackFunc>
  ```
  For Optimization to avoid Multi Definitions, using
  `inline`
  implementing a function in .h file\
  Then, compile void `BindNativeInputAction()` for the `BindAction`
  ```c++
  UCLASS()
	class WARRIOR_API UWarriorInputComponent: public UEnhancedInputComponent
	{
		GENERATED_BODY()
		
	public:
		template<class UserObject, typename CallbackFunc> 
		void BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func); 
	};
	
	template<class UserObject, typename CallbackFunc> 
	inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
	{
		checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));
		if (UInputAction* FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
		{
			BindAction(FoundAction,TriggerEvent,ContextObject,Func);
		}
	}
  ```
  After UE compiling, go to project setting, search inputcomponent\
  ![Screenshot 2025-01-13 204758](https://github.com/user-attachments/assets/06029077-f321-4ce2-8ade-2698de863e80)
  Changes to WarriorInputComponent.
- 6, binding input\
  In step 5, we created a new input component, then we needed to attach it to warriorherocharacter. If we do so, we need `SetupPlayerInputComponent` in Character.h
  ```c++
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
  ```
  When we start to implement the `Setupplayercomponent`, remember to check if `UDataAsset_InputConfig` exists (and attach it to `WarriorInputComponent`)
  ```c++
  checkf(InputConfigDataAsset, TEXT("Forgot to assign a valid data asset as input config."))
  ```
  Step1, implement the `AddMappingContext()` in `EnhancedInputSubsystemInterface.h`
  ```c++
  virtual void AddMappingContext(const UInputMappingContext* MappingContext, int32 Priority, const FModifyContextOptions& Options = FModifyContextOptions());
  ```
  By looking at its class, it is `class ENHANCEDINPUT_API IEnhancedInputSubsystemInterface`, because it is `interface`, so using a global search for `public IEnhancedInputSubsystemInterface`, we get `class ENHANCEDINPUT_API UEnhancedInputLocalPlayerSubsystem: public ULocalPlayerSubsystem, public IEnhancedInputSubsystemInterface`, As we can see, only ULocalPlayerSubsystem is not interface, so go to its definition, we can see `class ULocalPlayerSubsystem: public USubsystem`, inside the public, I can see it is under the `template <LocalPlayer>`, so go to LocalPlayer.h, search `Subsystem`,
  ```c++
  static FORCEINLINE TSubsystemClass* GetSubsystem(const ULocalPlayer* LocalPlayer)
  ```
Then the rest is easy, we need to find `LocalPlayer` by `GetController<> -> GetLocalPlayer()`
```c++
ULocalPlayer* LocalPlayer = GetController<APlayerController>() -> GetLocalPlayer();
UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);                                                                                                       

check(Subsystem);

Subsystem -> AddMappingContext(InputConfigDataAsset -> DefaultMappingContext, 0);
```
Step2, Cast `WarriorInputComponent` to `PlayerInputComponent` by using `CastChecked<>`
```c++
UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);
```
![Screenshot 2025-01-16 101052](https://github.com/user-attachments/assets/99afdbbb-efeb-4d7c-bf37-1d5acd315de9)
\
Step3, create the Input_Move and Input_Look functions\
`AddMovementInput()` is used to enable the controller's value access.
```c++
void AWarriorHeroCharacter::Input_Move(const FInputActionValue &InputActionValue)
{
    const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection,MovementVector.Y);
	}
	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection,MovementVector.X);
	}
}
void AWarriorHeroCharacter::Input_Look(const FInputActionValue &InputActionValue)
{
    const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	
	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
```
Step4, BindNativeInputAction
```c++
inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
```
```c++
WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_Move,ETriggerEvent::Triggered,this,&ThisClass::Input_Move);
WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_Look,ETriggerEvent::Triggered,this,&ThisClass::Input_Look);
```
Step5, Enable Walking\
[Watch the video](Media/DataAccess.mp4)
- 7, Character Anime Instance
![Screenshot_20250116_112246_Samsung capture](https://github.com/user-attachments/assets/f60d9401-aeca-4aeb-917d-53d97313bcae)
[Watch the Video](Media/AnimeInstance.mp4)
`NativeInitializeAnimation`: set up any required data or references for the animation to work properly.\
`NativeThreadSafeUpdateAnimation(float DeltaSeconds)`: safely update animation-related data every frame in a thread-safe and performance-optimized way.\
In `WarriorCharacterAnimInstance.cpp`, we need to get the character's speed, and then update the animation based on the speed.
```c++
void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
    OwningCharacter = Cast<AWarriorBaseCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D()>0.f;
}
```
Then, create a new blueprint animation.
[Watch the Video](Media/ABP_HERO.mp4)

- 8, Hero idle to Walk Anim
  Step1, create a new folder names BlendSpace, and create a new blendspace 1d call BS_UnarmedLocolmotion;\
  Step2, ![Screenshot 2025-01-19 110612](https://github.com/user-attachments/assets/2f982797-cbea-4418-b199-b2f5099cc155)\
  Step3, ![Screenshot 2025-01-19 112736](https://github.com/user-attachments/assets/fe0ad523-9298-4619-85e7-bab465f62ee4)\
  Step4, ![Screenshot 2025-01-19 112728](https://github.com/user-attachments/assets/8cfb0d47-19b7-41b0-afc9-d9c66c0b4f63)

- 9, Hero relax Anim Instan
  Step1, ![Screenshot 2025-01-19 120812](https://github.com/user-attachments/assets/cbf43887-83a3-417f-ab73-f77e3a7ce981)\
  Step2, create a new random sequence, add two new entities, and change their change to play is 0.5\
  ![Screenshot 2025-01-19 120830](https://github.com/user-attachments/assets/96b0c1cc-11ed-44ea-b585-9a2982d0d1bf)\
  Step3, In `WarriorHeroAnimInstance.h`, use `NativeInitializeAnimation()` to initial data and update status by different deltaTime to enable/disable relax in `NativeThreadSafeUpdateAnimation`
  ```c++
  void UWarriorHeroAnimInstance::NativeInitializeAnimation()
	{
	    Super::NativeInitializeAnimation();

    		if (OwningCharacter)
		{
			OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
		}
	}
  void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
	{
	    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
		if (bHasAcceleration)
		{
			IdleElpasedTime = 0.f;
			bShouldEnterRelaxState = false;
		}
		else
		{
			IdleElpasedTime += DeltaSeconds;
			bShouldEnterRelaxState = (IdleElpasedTime >= EnterRelaxtStateThreshold);
		}
	}
- 10, Gameplay Ability System\
  Purpose: designed to handle gameplay mechanics like abilities, effects, and attributes.\
  ![Screenshot_20250123_093537_Samsung capture](https://github.com/user-attachments/assets/6bca1ea9-3580-4ef5-9166-1a6cf327955c)\
  Step1, enable the gameplay system plugin\
  ![Screenshot 2025-01-23 093439](https://github.com/user-attachments/assets/87399900-6750-4345-87a6-d59fe74aeed5)\
  In Warrior.Build.cs file, add `"GameplayTasks"` inside the code `PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject",...`
  Step2, create AbilitySystem in c++\
  Create a new public name `WarriorGameplayAbilityComponent` in the new folder `AbilitySystem`\
  Step3, create a new c++ AttibuteSet\
  Create a new public name `WarriorAttributeSet` in the new folder `AbilitySystem`\
  Step4, Add these new c++ class into `WarriorBaseCharacter.h`\
  ```c++
  protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;
  public:
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const {return WarriorAbilitySystemComponent;}
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const {return WarriorAttributeSet;}
  ```
  Step5, Attach them in the `WarriorBaseCharacter.cpp`\
  ```c++
  #include "AbilitySystem/WarriorAbilitySystemComponent.h"
  #include "AbilitySystem/WarriorAttributeSet.h"

  AWarriorBaseCharacter::AWarriorBaseCharacter()
  { ...
  	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));
	WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));
  ```
  Step6, Initialize GAS in PossessedBy()\
  In Character.h, `ENGINE_API virtual void PossessedBy(AController* NewController) override;` sets their default values\
  In WarriorBaseCharacter.h\
  ```c++
  protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface
  ```
  In WarriorBaseCharacter.cpp\
  Using `InitAbilityActorInfo()` to set The logical "owner" of the AbilitySystemComponent and he "physical" actor that performs actions\
  ```c++
  void AWarriorBaseCharacter::PossessedBy(AController *NewController)
  {
	Super::PossessedBy(NewController);

	if(WarriorAbilitySystemComponent)
	{
		WarriorAbilitySystemComponent -> InitAbilityActorInfo(this, this);
	}
   }
  ```
  Step7, Implementing IAbilitySystemInterface\
  The IAbilitySystemInterface is an interface provided by Unreal Engine's Gameplay Ability System (GAS) that acts as a bridge to let other systems know that an actor has an associated UAbilitySystemComponent, Without this interface, you'd need to manually cast 
  your actor class (e.g., AWarriorBaseCharacter) every time you want to access its AbilitySystemComponent, which is less modular and less extensible.\
  In AbilityInterface.h, `class GAMEPLAYABILITIES_API IAbilitySystemInterface{ `, we need its `virtual UAbilitySystemComponent* GetAbilitySystemComponent() const = 0;`
  So in WarriorBaseCharacter.h\
  ```c++
  class WARRIOR_API AWarriorBaseCharacter : public ACharacter, public IAbilitySystemInterface
  { ...
  public:
	AWarriorBaseCharacter();

	//~ Begin IAbilitySystemInterface Interface.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	//~ End APawn Interface Interface
  ```
  In warriorBaseCharacter.cpp\
  ```c++
  UAbilitySystemComponent *AWarriorBaseCharacter::GetAbilitySystemComponent() const
  {
    return GetAbilitySystemComponent();
  }
  ```
  Step8, Detect does the GAS work in the main character\
  In warriorHeroCharacter.h
  ```c++
  protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface
  ```
  In WarriorHeroCharacter.cpp
  ```c++
  #include "AbilitySystem/WarriorAbilitySystemComponent.h"
  ...
  void AWarriorHeroCharacter::PossessedBy(AController* NewController)
  {
	Super::PossessedBy(NewController);
	if (WarriorAbilitySystemComponent && WarriorAttributeSet)
	{	
		const FString ASCText = FString::Printf(TEXT("Owner Actor: %s, AvatarActor: %s"),*WarriorAbilitySystemComponent->GetOwnerActor()->GetName(),*WarriorAbilitySystemComponent->GetAvatarActor()->GetName());
		
		Debug::Print(TEXT("Ability system component valid. ") + ASCText,FColor::Green);
		Debug::Print(TEXT("AttributeSet valid. ") + ASCText,FColor::Green);
	}
  }
  ```
- 11, Build the gameplay Ability\
  From step 10, we build the abilitysystemcomponent, in here, we need to give the ability to the character, which requires new stuff to attach into abilitysystemcomponent\
  ![Screenshot_20250124_102133_Samsung capture](https://github.com/user-attachments/assets/5e1f738a-5e0c-40b0-812f-bbdb7fd91251)
  Step1, create a new c++ `gameplayability` name `warriorgameplayability` in a new folder `abilities`\
  Step2, compiler the GameplayAbility policy
  ```c++
  UENUM(Blueprint)
  UENUM(Blueprint)
  enum class EWarriorAbilityActivationPolicy : uint8
  {
	OnTriggered,
	OnGiven
  };
  UCLASS()
  class WARRIOR_API UWarriorGameplayAbility : public UGameplayAbility
  {
	GENERATED_BODY()
	
  protected:

	UPROPERTY(EditDefaultOnly, Category = "WarriorAbility")
	EWarriorAbilityActivationPolicy AbilityActivationPolicy = EWarriorAbilityActivationPolicy::OnTriggered;
  ```
  Step3, Given Ability to Character and End Ability\
  In GameplayAbility.h, we need two functions `OnGiveAbility` and `EndAbility`.\
  Step4, Implement these two functions\
  ```c++
  void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilitySpec &Spec)
  {
    Super::OnGiveAbility(ActorInfo, Spec);
    
    if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
    {
        if (ActorInfo && !Spec.IsActive())
        {
            ActorInfo -> AbilitySystemComponent -> TryActivateAbility(Spec.Handle);
        }
    }
  }
  void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
  {
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
    {
        if (ActorInfo)
        {
            ActorInfo -> AbilitySystemComponent -> ClearAbility(Handle);
        }
    }
  }
  ```
  Step5, Create a new blueprint\
  go to ue's content, create Shared/GameplayAbility folder, then select gameplay/gameplayability blueprint, rename GA_Shared_SpawnWeapon.\
  change the Warrior Ability|Ability Activation Policy to `On Given`.
- 12, Create weapon Class\
  In the process of setting up the weapon class, we need to understand the weapon class structure:\
  `WarriorWeaponBase`(Handles damage detection) -> `WarriorHeroWeapon`(Weapon data unique to player)\
  In C++, create a new class Actor called `WarriorWeaponBase` under the folder items/weapons\
  Then, create a child class `WarriorHeroWeapon`\
  In WarriorWeaponBase.h
  ```c++
  protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	UBoxComponent* WeaponCollisionBox;
  public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox;}
  ```
  In WarriorWeaponBase.cpp
  ```c++
  AWarriorWeaponBase::AWarriorWeaponBase()
  {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  ```
  After compiling that, create a blueprint based on `WarriorHeroWeapon`, called `BP_HeroWeapon` under the folder playercharacter/HeroWeapon\
  Then create a child class name `BP_HeroAxe`, attach the static mesh, and adjust its size.\
- 13, Set Up Hero Weapon\
  Step 1,\
  In shared/GA_Shared_GameplayAbility, right-click, in blueprint, `Spawn Actor from class`, \
  create a new variable, named `WeaponClassToSpawn`, then changes its type to `Warrior Weapon Base`, and its `Change to:` should be `Class Reference`\
  Spilt the `Spawn Transformation`\
  Collision Handling should be `Try to adjust, but always spawn`\
  Attempt the `Spawn Actor from class`. Create `Get Avatar from actor info`, connect it to `Owner`, and use `cast to pawn`(convert to pure), to connect to investigator\
  Connect `Event ActivateAbility` to `SpawnActor`\
  create `Is Valid`, drag `return value`, create `Attach Actor To Component`\
  Connect `GetSkeletalMeshComponentFromActorInfo` to `Parent`\
  Promote a new variable for `Socket`, names `Socket Name Attach To`\
  Change `Location Rule` to `Snap to Target`\
  Change `Scale Rule` to `Keep World`\
  Connect `Attach Actor To Component` to `End Ability`(Need to be created)\
  ![Screenshot 2025-01-26 103047](https://github.com/user-attachments/assets/6c1ce246-0a2f-46d4-917e-43db819d7197) \
  Step 2,
  create the child of `GA_Hero_Weapon_Axe`, move it to PlayerCharacter/GameplayAbility.\
  Click the new child, change its `weapon class to spawn` to `AXE`\
  Create a new socket \
  ![Screenshot 2025-01-26 104356](https://github.com/user-attachments/assets/37213573-bf37-4fd9-9f53-3a898a249847) \
  change the socket name \
  ![Screenshot 2025-01-26 104545](https://github.com/user-attachments/assets/7e4a3256-8bb9-4623-95cc-759ff04964ee)
- 14, Strat Up Data\
  ![Screenshot_20250126_111728_Samsung capture](https://github.com/user-attachments/assets/f8b865a8-ea8b-4d04-9f35-7c50e95d1f90) \
  Create a new DataAsset c++ class into DataAssets/StartupData, named `DataAsset_StartupDataBase`\
  In DataAsset_StartupDataBase.h
  ```c++
  	GENERATED_BODY()
  public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);
  protected:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UWarriorGameplayAbility > > ActivateOnGivenAbilities;	
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UWarriorGameplayAbility > > ReactiveAbilities;
	void GrantAbilities(const TArray< TSubclassOf < UWarriorGameplayAbility > >& InAbilitiesToGive,UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);
  ```
  In DataAsset_StartupDataBase.cpp
  ```c++
  void UDataAsset_StartupDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent *InASCToGive, int32 ApplyLevel)
  {
    check(InASCToGive);

    GrantAbilities(ActivateOnGivenAbilities,InASCToGive,ApplyLevel);
	GrantAbilities(ReactiveAbilities,InASCToGive,ApplyLevel); 
  }
  void UDataAsset_StartupDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>> &InAbilitiesToGive, UWarriorAbilitySystemComponent *InASCToGive, int32 ApplyLevel)
  {
    if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UWarriorGameplayAbility>& Ability : InAbilitiesToGive)
	{
		if(!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
        
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		InASCToGive->GiveAbility(AbilitySpec);
	}
  }
  ```
  Create a derived class name `DataAsset_HeroStartUpData`, and create a new dataset blueprint name `DA_Hero` in the PlayerCharacter folder.\
  ![Screenshot 2025-01-26 120915](https://github.com/user-attachments/assets/c5af4a38-cfd9-4ffb-a928-f62f4944bc95)
- 15, Synchronous Loading \
  In WarriorBaseCharacter.h, we use soft reference to enable the DataAsset usage.
  ```c++
  protected:
  	...
  	UWarriorAttributeSet* WarriorAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;
  ```
  ![Screenshot_20250126_132904_Samsung capture](https://github.com/user-attachments/assets/e1c39258-a79d-4548-a8b2-be3d3f332b71) \
  In WarriorHeroCharacter.cpp, we need to load this soft reference into the `poessessedby()`,
  ```c++
  void AWarriorHeroCharacter::PossessedBy(AController* NewController)
  {
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
  }
  ```
  Then, we need a Marco to highlight if the `poessessedby()` does not run successfully. So in WarriorBaseCharacter.cpp,
  ```c++
  void AWarriorBaseCharacter::PossessedBy(AController *NewController)
  {
    ...
    if (WarriorAbilitySystemComponent)
	{
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this,this);
		ensureMsgf(!CharacterStartUpData.IsNull(),TEXT("Forgot to assign start up data to %s"),*GetName());
	}
  ```
  After that, \
  ![Screenshot 2025-01-26 135934](https://github.com/user-attachments/assets/aae2a8e4-70dd-41ef-92b4-e866f2e3ff60)

# 2, Combo System
 - In this part, majorly we will do things: Weapon Equiping and Combo System
 - Weapon Equipping: Retrieve the spawned weapon, A new Gameplay Ability, Ability Input Action, Armed Locomotion, Unequip
 - Combo System: Moves can be easily added or removed, Animation Indepent, Light/Heavy Attack, Communication, Damaged scaled by combo count
 - 1, Create hero combat component\
   ![Screenshot_20250130_090317_Samsung capture](https://github.com/user-attachments/assets/85094517-76aa-4e8a-bdc4-3a361ab03223)
   create a new c++ Actor Component class named `PawnExtenComponentBase`
   ```c++
   UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
   class WARRIOR_API UPawnExtensionComponentBase : public UActorComponent
   {
	GENERATED_BODY()

   protected:
	template <class T>
	T* GetOwningPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' Template Parameter to GetPawn must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}
	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	template <class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' Template Parameter to GetController must be derived from AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}		
   };
   ```
   Then, create a new child c++ class `PawnCombatComponent` in components/Combat\
   and create a new child base on c++ class `PawnCombatComponent`, name `HeroCombatComponent`\
   First, add this `#include "Components/Combat/HeroCombatComponent.h"` in `WarriorHeroCharacter.cpp`\
   Secondly, add `class UHeroCombatComponent;` in `WarriorHeroCharacter.h`\
   Third, add the rest in `WarriorHeroCharacter.h`
   ```c++
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
   UHeroCombatComponent* HeroCombatComponent;	
   public:
   FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
   ``` 
   Fourth, add `HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));` in the  `WarriorHeroCharacter.cpp`\
- 2, Registered weapons by its tags\
  In the first step, we must create a data structure to store `Required registered ` weapons. In PawnCombatComponent.h, we use `TMap<typename A, tyname B,...>` to store weapons.\
  For A, we need tags, which come from `FGameplayTag`, by checking the `WarriorGameplayTags.h`, we can use its header `#include "NativeGameplayTags.h"`,\
  For B, we need WeaponActor, which comes from `AWarriorWeaponBase* `, by `WarriorWeaponBase.cpp`, we need to class its type and include its header .h.
  ```c++
  TMap<FGameplayTag,AWarriorWeaponBase*> CharacterCarriedWeaponMap;
  ```
  Second step, we need to register spawned weapons with the character, retrieve carried or equipped weapons based on their Gameplay Tags, and manage the character's weapon inventory.\
  `RegisterSpawnedWeapon`: register the weapon matched with its tag.\
  `GetCharacterCarriedWeaponByTag`: retrieve the specific weapon by characters.\
  `CurrentEquippedWeaponTag`: Keep track of the current weapon.\
  `GetCharacterCurrentEquippedWeapon`: Get the current weapon.\
  ```c++
  public:
	UFUNCTION(BlueprintCallable, Category = Category = "Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false); // fasle for hero, true for enemy

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;
  ```
  The third step, implement those UFunctions.\
  In `RegisterSpawnedWeapon`, 
  we need `checkf()` and `check()` to clarify whether these tags and weapon actor exists, and use `.Emplace()` to register them to the `CharacterCarriedWeaponMap`, last check if this is a required equipped weapon for the main character, if so, then do.
  ```c++
  void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
  {
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A named named %s has already been added as carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}
  }
  ```
  In `GetCharacterCarriedWeaponByTag`,
  we use two functions to seek the required weapon actor, `.conatins()` and `.find()`
  ```c++
  if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
  {
	if (AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
	{
		return *FoundWeapon;
	}
  }
  return nullptr;
  ```
  In  `GetCharacterCurrentEquippedWeapon`:,
  we need to use `GetCharacterCarriedWeaponByTag`, if the tag does not exist, then it can't get the weapon actor, if it does, return that actor
  ```c++
  if (!CurrentEquippedWeaponTag.IsValid())
  {
	return nullptr;
  }
  return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
  ```
  The fourth step, create a tag for the weapon\
  In `WarriorGameplayTags` .h and .cpp, add `WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe)` , `UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Axe, "Player.Weapon.Axe")`.\
  The fifth step is debugging if the tag or weapon actor has an issue.\
  In `PawnCombatComponent.cpp`,
  ```c++
  #include "WarriorDebugHelper.h"
  ...
  const FString WeaponString = FString::Printf(TEXT("A weapon named: %s has been registered using the tag %s"), *InWeaponToRegister->GetName(), *InWeaponTagToRegister.ToString());
  Debug::Print(WeaponString);
  ```
  In the last step, call out this ability in `WarriorGamePlayability`\
  In .h,
  ```c++
  UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
  UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;
  ```
  In .cpp,
  ```c++
  UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
  {
    return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
  }
  ```
  Purpose: allowing the caller to access the combat-related data or functionality provided by this component.\
  Connect the blueprint in GamePlayAbility/GA_Shared_SpawnWeapon, \
  ![Screenshot 2025-01-31 113226](https://github.com/user-attachments/assets/3ab3e1c8-d55a-4968-a023-bd26b36a2d2e)\
  ![Screenshot 2025-01-31 113632](https://github.com/user-attachments/assets/4b5e3828-561e-4a30-9bfb-7ec97e031f77)
- 3, Add a new GameplayAbility for the spawned equipped weapon\
  ![Screenshot_20250202_154801_Samsung capture](https://github.com/user-attachments/assets/baf29f0b-820a-4ad4-b02f-6e8a114b58fb)
  Purpose: Creating helper function for `GA_Hero_EquipedAxe blueprint`.\
  In `WarriorGameplayAbility.h`
  ```c++
  class UWarriorAbilitySystemComponent;
  ...
  UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
  UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;
  ```
  In `WarriorGameplayAbility.cpp`, we use `CurrentActorInfo` in `GameplayAbility.h` and `GameplayAbilityTypes.h`
  ```c++
  UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
  {
    return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
  }
  ```
  First Step, create a child class of `WarriorGameplayAbility`,\
  Second Step, implement them. (The TweakObjectPtr struct contains a pointer to an AWarriorHeroCharacter object and provides methods to tweak the character's health and strength.)\
  In .h,
  ```c++
  public:
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorHeroController* GetHeroControllerFromActorInfo();
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();
  private:
	TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
	TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;
  ```
  In .cpp,
  ```c++
  AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
  {
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get() : nullptr;
  }
  AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
  {
	if (!CachedWarriorHeroController.IsValid())
	{
		CachedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}

	return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get() : nullptr;
  }
  UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
  {
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
  }
  ```
  The third step, create the blueprint.\
  ![Screenshot 2025-02-02 162851](https://github.com/user-attachments/assets/d511f3d0-c90a-416d-96ca-6a283c14d14e)
- 4, create equip/unequip weapon tags, mapping data structure, and attach them to the map\
  First, in `WarriorGameplayTags.h`
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_EquipAxe);
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_UnequipAxe);
  ```
  In `WarriorGameplayTags.cpp`
  ```c++
  UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipAxe, "InputTag.EquipAxe");
  UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequipAxe, "InputTag.UnequipAxe");
  ```
  Second, in `DataAsset_Inputconfig.h`
  ```c++
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorInputActionConfig> AbilityInputActions;
  ```
  Third, Go to `WarriorInputComponent.h`
  ```c++
  template<class UserObject,typename CallbackFunc>
	void BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig,UserObject* ContextObject,CallbackFunc InputPressedFunc,CallbackFunc InputRelasedFunc);
  ```
  In `WarriorInputComponent.cpp`, use struct FWarriorInputActionConfig's reference: AbilityInputActionConfig's map
  ```c++
  template<class UserObject, typename CallbackFunc>
  inline void UWarriorInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig, UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputRelasedFunc)
  {
	for (const FWarriorInputActionConfig& AbilityInputActionConfig : InInputConfig->AbilityInputActions)
	{
		if (!AbilityInputActionConfig.IsValid()) continue;
		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputPressedFunc, AbilityInputActionConfig.InputTag);
		BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InputRelasedFunc, AbilityInputActionConfig.InputTag);
	}
  }
  ```
  Fourth, in `WarriorInputComponent.h`, create a helper function to identify valid, checks the tag and tag action is valid or not
  ```c++
  bool IsValid() const
	{
		return InputTag.IsValid() && InputAction;
	}
  ```
  Fifth, change the name of `IE_Jump` to `IE_EquipAxe`, in the Data_InputCofig, do it\
  ![Screenshot 2025-02-02 171203](https://github.com/user-attachments/assets/26fc7ff4-c82d-498c-b174-a976e2987fe8)
- 5, Hero Ability Set in Startup_Data\
  ![Screenshot_20250203_092221_Samsung capture](https://github.com/user-attachments/assets/71c13949-ccff-4f20-b8bf-b94f5e4e8d72)
  First, To allow the character to gain this startup_ability, we need to add this in `DataAsset_HeroStartupData.h`\
  To do that, we require struct `FGameplayTag` from `GameplayTagContainer.h`\
  Then, we need a subclass of the WarriorGameplayAbility, because it is under the public UGameplayAbility\
  After that, a valid function is required.
  ```c++
  USTRUCT(BlueprintType)
  struct FWarriorHeroAbilitySet
  {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;

	bool IsValid() const;
  };
  ```
  Second, implement the `IsValid()`\
  ```c++
  bool FWarriorHeroAbilitySet::IsValid() const
  {
	return InputTag.IsValid() && AbilityToGrant;
  }
  ```
  Third, create an array to sort the warriorabilities, FWarriorHeroAbilitySet struct has two parts: tags and the ability matched to the tag.
  ```c++
  private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets;
  ```
  Fourth, recall dataasset_startup's attaching function `GiveToAbilitySystemComponent`
  ```c++
  virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;
  ```
  Fifth, implement it.
  ```c++
  Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

  for (const FWarriorHeroAbilitySet& AbilitySet: HeroStartUpAbilitySets)
  {
    if (!AbilitySet.IsValid()) continue;

    FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
    AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
    AbilitySpec.Level = ApplyLevel;
    AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
    InASCToGive->GiveAbility(AbilitySpec);
  }
  ```
- 6, Binding ability input\
  First, In WarriorHeroCharacter.h
  ```c++
  void Input_AbilityInputPressed(FGameplayTag InInputTag);
  void Input_AbilityInputReleased(FGameplayTag InInputTag);
  ```
  Second, In Warriorabilitysystemcomponent.h
  ```c++
  public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);
  ```
  Third, Implement the `OnAbilityInputPressed()`, using `HasTagExact` for the exact check, and `TryActivateAbility()` for activate activity. 
  ```c++
  void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
  {
	if (!InInputTag.IsValid())
	{
		return;
	}
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		TryActivateAbility(AbilitySpec.Handle);
	}
  }
  ```
  Fourth, Implement in WarriorHeroCharacter.cpp
  ```c++
  void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
  {
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
  }

  void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
  {
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
  }
  ...
  WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
  ```
- 7, Play Montage And Wait\
  Goal: Create an upper body Montage and attach it.
  In Hero_Axe_Equip,\
  ![Screenshot 2025-02-07 174316](https://github.com/user-attachments/assets/b24971cd-ca82-4a89-a964-bcf17a25d96a)\
  ![Screenshot 2025-02-07 174352](https://github.com/user-attachments/assets/dc54e0ac-3579-44ae-a34c-9ad9e3f14f85)\
  In ABP_Hero, remember to add a new layer of branch bone\
  ![Screenshot 2025-02-07 174248](https://github.com/user-attachments/assets/4da4825e-fbd1-4b23-a234-36f096890bdf)\
  In GA_Hero_Equip_Axe,\
  ![Screenshot 2025-02-07 174255](https://github.com/user-attachments/assets/6b554eb1-0684-4944-a980-85e84211bc34)
- 8, wait for gameplay event\
  First, Create a new blueprint named `AN_SendGamePlayToOwner` from AnimeNotify\
  ![Screenshot 2025-02-07 181830](https://github.com/user-attachments/assets/697fdeaa-43ca-44bf-a66f-4e259ae71639)\
  And then,\
  In WarriorGameplayTags.h,
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
  ```
  In WarriorGameplay.cpp,
  ```c++
  UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Axe, "Player.Event.Equip.Axe");
  UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Axe, "Player.Event.Unequip.Axe");
  ```
  Second, In AM_Hero_Equip_Axe, add that notify
  ![Screenshot 2025-02-07 181957](https://github.com/user-attachments/assets/0b3f0ea4-9a88-455b-b237-ae39ad7238d8)\
  Third, In GA_Hero_Equiped_Axe
  ![Screenshot 2025-02-07 182124](https://github.com/user-attachments/assets/38a3b3a5-af50-4eff-83ce-87f0cdf0b982)\
  Test\
  Fourth, Add a new socket in the hand_r and finish the rest of the Gameplay Ability Graph\
  ![Screenshot 2025-02-10 090328](https://github.com/user-attachments/assets/640427de-69aa-49ad-8991-1f15e2f10759)\
  ![Screenshot 2025-02-10 090340](https://github.com/user-attachments/assets/57672762-ce26-46be-aedb-e0267cf2ef44)
- 9, Animation Layer Interface\
  In this part, we need to satisfy Armed Locomotion. So we need to understand how to use the Animation Layer for different weapons.\
  Create a new Animation Layer Interface named ALI_Hero, and rename the node `ArmedLocomotionState`\
  Next, In ABP_hero, select the class setting, and add the new interface in it.\
  using blueprint: blend poses by bool, property assets(combat component -> weapon tag), is valid\
  ![Screenshot 2025-02-10 093943](https://github.com/user-attachments/assets/2e432fce-e480-4a7c-9504-6d2f8cb781ed)\
  Last, set the combat component gameplay tag after the character equips his weapon.\
  ![Screenshot 2025-02-10 095124](https://github.com/user-attachments/assets/27b5d420-508f-47c8-bf87-1bcfca6d417b)
- 10, Master Anim Layer\
  Purpose: create a major hero locomotion animation layer, and split it into different with/without weapon locomotions.\
  The first step is to create a new blueprint animation, named Master_AnimLayer_Hero, the parent class is WarriorHeroLinkedAnimLayer. Click on it, class setting, and add the ALI_Hero interface. \
  Then double click the Armed Locomotion State, add blendspace player, click this new node, let coordinates-> y -> expose as a pin is unchecked, and settings-> blend space -> expose as a pin is checked.\
  Promote the blend space to a new variable: Default Locomotion Blend Space, Because ABP_hero class is `Warrior Hero Anim Instance`, but Master_AnimLayer_Hero class is `Warrior Hero Linked Anim Layer`, so we need to fix in the c++\
  In WarriorAnimLinkedLayer.h
  ```c++
  public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UWarriorHeroAnimInstance* GetHeroAnimInstance() const;
  ```
  and implement it
  ```c++
  UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
  {
	return Cast<UWarriorHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
  }
  ```
  Go back to the Master_AnimLayer_Hero, add a new property asset, and choose GetHeroComponent->GroundSpeed.
  The second step is to create a new child blueprint based on the Master_AnimLayer_Hero, we need to add a new 1D animation blend space for it.\
  Third step, create a new blendspace, named Default_Locomotion_Axe, Then attach it to the `Master_AnimLayer_Hero`.
  ![Screenshot 2025-02-10 103443](https://github.com/user-attachments/assets/36a4b72e-17ec-4005-95a9-222b5df8a336)
- 11, Link Anim Layer\
  Create a new None C++ called WarriorstructTypes. The purpose is to neatly package all warrior-related data. The use of UPROPERTY with appropriate specifiers ensures that these properties are editable and accessible in Blueprints.\
  In WarriorStructTypes.h,
  ```c++
  #include "WarriorStructTypes.generated.h"

  class UWarriorHeroLinkedAnimLayer;

  USTRUCT(BlueprintType)
  struct FWarriorHeroWeaponData
  {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;
  };
  ```
  In HeroCombatComponent.h,
  ```c++
  class AWarriorHeroWeapon;
  ...
  public:
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const;
  ```
  In WarriorHeroWeapon.h,
  ```c++
  #include "WarriorTypes/WarriorStructTypes.h"
  ...
  public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FWarriorHeroWeaponData HeroWeaponData;
  ```
  In HeroCombatComponent.cpp,
  ```c++
  #include "Items/Weapons/WarriorHeroWeapon.h"

  AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
  {   
    return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
  }
  ```
  Then, click on the BP_Hero_Axe, and add the AnimLayer_Axe to the Hero Weapon Data.\
  ![Screenshot 2025-02-11 095133](https://github.com/user-attachments/assets/c9a1e3d6-5ddc-436f-8dac-4d5b384ac7bf)\
  Later, link the animlayer,\
  ![Screenshot 2025-02-11 100006](https://github.com/user-attachments/assets/71bffd0e-9e99-403e-8067-a71cb743c543)
- 12, Default Weapon Ability For Unequip weapons\
  First, add new ability tags to WarriorGameplayTags.\
  In .h, 
  ```c++
  /** Player tags **/
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
  ```
  In .cpp,
  ```c++
  /** Player tags **/
  UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Axe, "Player.Ability.Equip.Axe");
  UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Axe, "Player.Ability.Unequip.Axe");
  ```
  In GA_Equipped_Axe, uses all tags.\
  ![Screenshot 2025-02-12 181318](https://github.com/user-attachments/assets/5dfaae17-232e-4e77-97d4-3f306d10024a)\
  Create a new GameplayAbility based on WarriorHerGampleAbility, named GA_Hero_Unequipped_Axe.\
  ![Screenshot 2025-02-12 181756](https://github.com/user-attachments/assets/e2f898d3-99c7-4e81-a003-ca71e36e15c5)\
  Then find an unequipped axe animation, and create a new blueprint montage. 
  The next step is similar to adding a blueprint in the GA_Unequiped_Axe, add a new input, add the new input action to the DA_InputConfig.\
  Second Thing: Where should I grant this unequipped weapon axe ability? We can add it when the game starts or after the weapon is attached.\
  In this case, we let this ability set into the weapon.\
  ![Screenshot_20250212_185731_Samsung capture](https://github.com/user-attachments/assets/63c5f89d-0ccd-4d5b-9327-0a9778b2db3a)\
  ![Screenshot 2025-02-12 191316](https://github.com/user-attachments/assets/1685796d-9287-467e-ac21-2dd5cb5f13d9)\
  ![Screenshot 2025-02-12 191322](https://github.com/user-attachments/assets/07d4f8ff-6c70-49dd-b333-cf79a166588d)
- 13, Grant Weapon Ability\
  In this part, we only do three things: 1, Play the Animation; 2, Attach the Input Mapping; 3, Grant the exact ability.\
  The first step, enable the play animation function and input mapping:\
  In GA_Hero_Equipped_Axe, create a new function and add a new Warrior Weapon Base object interface\
  ![Screenshot 2025-02-16 181555](https://github.com/user-attachments/assets/3a760c13-25c5-4fd5-a00f-c280e43ea805)\
  ![Screenshot 2025-02-16 181604](https://github.com/user-attachments/assets/9fa581bd-0952-471a-8119-1a0a88b8c410)\
  The second step, fill up the WarriorAbilityComponentSystem,\
  In .h,
  ```c++
  UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel = "1"))
  void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);
  ```
  In .cpp,
  ```c++
  void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
  {
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return;
	}

	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
  }
  ```
  In the last step, attach the link.\
  ![Screenshot 2025-02-16 182859](https://github.com/user-attachments/assets/489d69a5-d348-4c25-b3c7-42817c87ee62)
- 14, Handle unequip axe\
  In the first step, do the same as the equipped axe\
  ![Screenshot 2025-02-16 190530](https://github.com/user-attachments/assets/e654d655-96c2-49c1-93d1-71a45d835c28)\
  Second, In WarriorHeroWeapon.h
  ```c++
  	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;

  private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
  ```
  In .cpp
  ```c++
  void AWarriorHeroWeapon::AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
  {
	GrantedAbilitySpecHandles = InSpecHandles;
  }

  TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
  {
	return GrantedAbilitySpecHandles;
  }
  ```
  Third, In WarriorAbilityComponent.h
  ```c++
  UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
  void RemovedGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
  ```
  In .cpp,
  ```c++
  void UWarriorAbilitySystemComponent::RemovedGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
  {
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}

	InSpecHandlesToRemove.Empty();
  }
  ```
  ![Screenshot 2025-02-20 084653](https://github.com/user-attachments/assets/c8a48fa7-86f5-46ff-93bb-378816d0c442)
- 15, Light Attack Ability\
  Here, we need to do the same steps: `Ability Tags`, `GameplayAbility Blueprint`, `Anim Montage`(skip it right now), `Ability Action`, `Grant Ability`.
- 16, Light Attack Combo\
  In GA_LightAttack_Master,\
  ![Screenshot 2025-02-21 174706](https://github.com/user-attachments/assets/2aaec406-1620-44a5-a13c-8aaf7ecf7d32)\
  Create a new variable `AttackMontageMap`, which is an integer, map, montage - object interference;\
  Make sure the `CurrentLightAttackCount`, `CombotCountRestTimeHandle` is private;\
  Enable the Instancing policy to `Instanced per Actor`\
  In GA_LightAttack_Axe,\
  ![Screenshot 2025-02-21 174819](https://github.com/user-attachments/assets/e5bb9b6f-7ffa-4104-89be-8c93e56e59d8)\
  Make sure those montages `EnableRootMotion` is checked;\
  We need to use a new slot, `Fullbody`, for these montages, and change their rate scale;\
  And add this new slot into ABP_Hero.\
  ![Screenshot 2025-02-21 175101](https://github.com/user-attachments/assets/11f5d1c7-7ac0-4cbe-a921-c8c216ab29a3)
- 17, Heavy Attack Ability\
  Same as 15, Light Attack Logic. But please don't forget to attach the heavy attack ability to the weapon.
- 18, Heavy Attack Logic\
  Same as 16.
- 19, BP Function Library\
  In this part, We need to create a combo link to enable the maximum damage, which means after 4 light attacks with 1 heavy attack can do more damage'\
  To do that, we need a Tag to show whether this link is finished or not.\
  Create a new BlueprintLibaray c++, names `WarriorFunctionLibrary`
  ```c++
  class UWarriorAbilitySystemComponent;

  UENUM()
  enum class EWarriorConfirmType : uint8
  {
	Yes,
	No
  };

  /**
   * 
   */
  UCLASS()
  class WARRIOR_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
  {
	GENERATED_BODY()

  public:
	static UWarriorAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType);
  };
  ```
  In .cpp
  ```c++
  #include "WarriorFunctionLibrary.h"
  #include "AbilitySystemBlueprintLibrary.h"
  #include "AbilitySystem/WarriorAbilitySystemComponent.h"

  UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
  {
	check(InActor);

	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
  }

  void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
  {
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
  }

  void UWarriorFunctionLibrary::RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
  {
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
  }

  bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
  {
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
  }

  void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType)
  {
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
  } 
  ```
  ![Screenshot 2025-02-21 191616](https://github.com/user-attachments/assets/1b47a9ec-ee5a-48ba-a364-8a4abf9d6354)
- 20, Jump To Finisher\
  Purpose: After the light attack combo, create a Jump To Finisher Tag; In the Heavy Attack, Check if exist the Jump To Finisher Tag.\
  ```c++
  UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToFinisher,"Player.Status.JumpToFinisher");
  ```
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_JumpToFinisher);
  ```
  ![Screenshot 2025-02-21 194343](https://github.com/user-attachments/assets/d9d4f737-beef-4f00-b26a-f1e1ed932016)\
  ![Screenshot 2025-02-21 194420](https://github.com/user-attachments/assets/406d41b3-49e7-46dd-92d6-7732d7288f7d)
- 21, Slow Motion And Sound FX\
  Create a new blueprint AnimNotiftState, Add two functions: `Received_NotifyBegin` and `Received_NotifyEnd`\
  ![Screenshot 2025-02-23 182136](https://github.com/user-attachments/assets/9a5aa625-d377-42da-bd14-cddec0091a03)\
  ![Screenshot 2025-02-23 182141](https://github.com/user-attachments/assets/f3d0658f-f142-4750-abc5-7b0c085dca36)\
  Then in those AM_montages, add your preferred sound FX and the time slow animnotify\
  ![Screenshot 2025-02-23 182212](https://github.com/user-attachments/assets/9571a183-90dc-4c72-a9e8-a6d5b2151ab7)
- Part 3\
  Summary:\
  ![Screenshot_20250224_085558_Samsung capture](https://github.com/user-attachments/assets/53aafe95-c576-4b3f-b533-1557dd02a349)\
- 22, Set Up Enemy Character\
  ![Screenshot_20250224_085951_Samsung capture](https://github.com/user-attachments/assets/c30f2110-30d5-4259-b0fd-f44e2c3abc12)\
  1, create a new C++ child based on `WarriorbaseonCharacter`, named WarriorEnemyCharacter\
  2, create a new C++ child based on `DataAsset_StartupDataBase`, named DataAsset_EnemyStartupDataBase\
  3, create a new C++ child based on `WarriorGameplayAbility`, named WarriorEnemyGameplayAbility\
  4, create a new C++ child based on `PawnCombatComponent`, named EnemyCombatComponent\
  5, In WarriorEnemyCharacter.h
  ```c++
  class UEnemyCombatComponent;
  /**
   * 
   */
  UCLASS()
  class WARRIOR_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
  {
	GENERATED_BODY()

  public:
	AWarriorEnemyCharacter();

  protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

  public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
  };
  ```
  In .cpp.
  ```c++
  #include "Characters/WarriorEnemyCharacter.h"
  #include "GameFramework/CharacterMovementComponent.h"
  #include "Components/Combat/EnemyCombatComponent.h"

  AWarriorEnemyCharacter::AWarriorEnemyCharacter()
  {
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
  }
  ```
  6, In WarriorEnemyGameplayAbility.h
  ```c++
  class AWarriorEnemyCharacter;
  class UEnemyCombatComponent;
  /**
   * 
   */
  UCLASS()
  class WARRIOR_API UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
  {
	GENERATED_BODY()

  public:
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorEnemyCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

  private:
	TWeakObjectPtr<AWarriorEnemyCharacter> CachedWarriorEnemyCharacter;
  ```
  In .cpp,
  ```c++
  #include "Characters/WarriorEnemyCharacter.h"

  AWarriorEnemyCharacter* UWarriorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
  {
	if (!CachedWarriorEnemyCharacter.IsValid())
	{
		CachedWarriorEnemyCharacter = Cast<AWarriorEnemyCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedWarriorEnemyCharacter.IsValid() ? CachedWarriorEnemyCharacter.Get() : nullptr;
  }

  UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
  {
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
  }
  ```
  7, Create a new blueprint based on the WarriorEnemyCharacter.\
  










  


  






