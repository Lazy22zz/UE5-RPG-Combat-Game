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
  In the process of setting up weapon class, we need to understand the weapon class structure:\
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
  After compiling that, create a blueprint base on `WarriorHeroWeapon`, called `BP_HeroWeapon` under the folder playercharacter/HeroWeapon\
  Then create a child class name `BP_HeroAxe`, and attach the static mesh and adjust its size.\



