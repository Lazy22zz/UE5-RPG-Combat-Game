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
  "T" in UE5, means the Type, such as TArray, TMap; \
  TObjectPtr is *hard* reference in UObject pointer; \
  ![Screenshot 2025-01-07 103243](https://github.com/user-attachments/assets/03a5a1ba-d070-4fa8-b3f8-6c3645a17bc0)
- 5, TSubclassof\
  Wrap up the required class type\
  Example:
  ```c++
  TArray< TSubclassOf < UWarriorGameplayAbility > > ActivateOnGivenAbilities;	
  ```
  We wrap up any types under `UWarriorGameplayAbility` that satisfies the required of `TArray<UClass *>`
- 6, Synchronous and  Asynchronous loading\
  In Unreal Engine C++, synchronous loading uses `LoadObject()` to load assets immediately, ensuring they are available before gameplay starts, while asynchronous loading uses `FStreamableManager::RequestAsyncLoad()` to load assets in the background without 
  blocking the main thread. Synchronous loading is ideal for critical assets like the player's character, ensuring smooth startup. In contrast, asynchronous loading is best for non-critical assets like enemies, optimizing performance and reducing initial load times.\
- 7, Basic GamplayEffectHandle code:
  ```c++
  // Create context
  FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
  // Set up context with relevant info
  ContextHandle.SetAbility(this);
  ContextHandle.AddSourceObject(SourceActor);
  // etc...

  // Create spec using that context
  FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
    EffectClass,
    Level,
    ContextHandle
  );

  // Configure spec as needed
  EffectSpecHandle.Data->SetSetByCallerMagnitude(...);
  // etc...

  // Then eventually apply the effect
  AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(EffectSpecHandle, TargetASC);
  ```
- 8, Automatic Storage, Static Storage,  Dynamic Storage:\
  Automatic storage: variables defined inside a function use `automatic storage`, which means the variables exist automatically when the function containing them is invoked, and expire when the function is terminated\
  Static Storage: exists throughout the execution of an entire program. Two ways to make a variable static: 1, define it externally, outside a function; 2, use a keyword `static` when declaring a variable\
  Dynamic Storage: 
  
- 9, `dynamic_cast` and `weak_ptr`:\
  ✅ Use std::weak_ptr when managing object lifetime safely without increasing reference counts.
  ✅ Use dynamic_cast only when you need to safely check and convert polymorphic types at runtime.
- 10, `checkf()`\
  Purpose: the game crashes and gets a report when it is false.\
- 11, pre constrcut and construct:\
  Pre Construct → Runs before the widget is fully created and can be previewed in the editor.\
  Construct → Runs after the widget is created and is used for regular initialization at runtime.
  
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
   Create a new c++ data asset renames DataAsset_InputConfig\
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
  After compiling, in character folder, goes to miscellaneous -> data asset -> Data Asset InputConfig, rename it DA_InputConfig, \
  Then using the default third-person provided by ue's Input folders' IMC_Default, set up the DA_InputConfig:
  ![Screenshot 2025-01-12 212251](https://github.com/user-attachments/assets/fec354c2-9419-4725-8446-d431265eafbc)
- 6, Custom Enhanced Input Component\
  Create a new enhance input component c++ under the public/Components/input folder, named WarriorInputComponent\
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
  Step1, create a new folder named BlendSpace, and create a new blendspace 1d called BS_UnarmedLocolmotion;\
  Step2, ![Screenshot 2025-01-19 110612](https://github.com/user-attachments/assets/2f982797-cbea-4418-b199-b2f5099cc155)\
  Step3, ![Screenshot 2025-01-19 112736](https://github.com/user-attachments/assets/fe0ad523-9298-4619-85e7-bab465f62ee4)\
  Step4, ![Screenshot 2025-01-19 112728](https://github.com/user-attachments/assets/8cfb0d47-19b7-41b0-afc9-d9c66c0b4f63)

- 9, Hero relax Anim Instant
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
- 23, Gruntling Guardian\
  1, Create a new child blueprint based on `BP_EnemyCharacter_Base` named BP_Gruntling_Base\
  2, Create a new child blueprint based on `BP_Gruntling_Base` named BP_Gruntling_Guardian\
  3, add mesh, adjust the capsule component(change it in BP_EnemyCharacter_Base can avoid later adjustment)\
  4, create a new animation blueprint, named ABP_Enemy_Base: template -> WarriorCharacterAnimstance\
  5, ![Screenshot 2025-02-24 100026](https://github.com/user-attachments/assets/e2acbf4d-f2f0-4f89-a9cd-05cea852a076)\
  6, Create a new animation blueprint child based on `ABP_Enemy_Base`, named ABP_Guadrain\
  7, create a new blend space for ABP_Guadrain using legacy -> blend space 1D, named BS_Guardian_Default\
  8, ![Screenshot 2025-02-24 100829](https://github.com/user-attachments/assets/42473028-072c-4f44-8f9c-3514f4b76602)\
  9, In BP_Gruntling_Guardian, attach the new ABP into anim class\
- 24, Asynchronous Loading\
  1, Create the DataAsset_EnemyStartupData\
  In DataAsset_EnemyStartupDataBase.h
  ```c++
  public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

  private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UWarriorEnemyGameplayAbility > > EnemyCombatAbilities;
  ```
  In DataAsset_EnemyStartupDataBase.cpp
  ```c++
  void UDataAsset_EnemyStartupDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
  {
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf < UWarriorEnemyGameplayAbility >& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			InASCToGive->GiveAbility(AbilitySpec);
		}
  }
  ```
  2, create a new data asset based on Data Asset Enemy Startup Data Base named DA_Guardian\
  3, Apply the Asynchronous Loading to the DA_Guardian\
  In Unreal Engine, synchronous loading is used for hero data to ensure the player's character and essential gameplay elements are fully loaded before the game starts, preventing delays or missing assets. In contrast, asynchronous loading is used for enemy data 
  to optimize performance by loading assets in the background, reducing initial load times, and allowing dynamic spawning without causing gameplay interruptions.\
  4, In WarriorEnemyCharacter.h
  ```c++
  protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEnemyCombatComponent* EnemyCombatComponent;

  private:
	void InitEnemyStartUpData();


  ```
  5, In WarriorEnemyCharacter.cpp
  ```c++
  void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
  {
	Super::PossessedBy(NewController);

	InitEnemyStartUpData();
  }

  void AWarriorEnemyCharacter::InitEnemyStartUpData()
  {
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UDataAsset_StartupDataBase* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);

					Debug::Print(TEXT("Enemy Start Up Data Loaded"), FColor::Green);
				}
			}
		)
	);
  }
  ```
  6, attach this new DA_Guardian in the BP_Gruntling_Guardian\
- 25, Spawn Enemy Weapon\
  1, rename `GA_Shared_SpawnWeapon` to `GA_Shared_SpawnWeapon_Base`\
  2, create a new child named `GA_Guadrain_SpawnWeapon`\
  3, In this new GA_Guadrain_SpawnWeapon, we have to fill up \
  ![Screenshot 2025-02-26 123823](https://github.com/user-attachments/assets/43a53cb4-6472-4002-ac9e-1b6d94a00a62)\
  4, create a new tag, in warriorGameplayTags.h
  ```c++
  	/** Enemy tags **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Weapon);
  ```
  5, In warriorGameplayTags.cpp
  ```c++
  	/** Enemy tags **/
	UE_DEFINE_GAMEPLAY_TAG(Enemy_Weapon,"Enemy.Weapon");
  ```
  6, Create a new BP_EnemyWeapon_Base based on blueprint WarriorWeaponBase\
  7, Create a new child BP_Guadrain_Weapon based on BP_EnemyWeapon_Base\
  8, Adjust the weapon and its collision box\
  9, add the new GA in the DA\
  ![Screenshot 2025-02-26 124900](https://github.com/user-attachments/assets/4a03dae9-eb41-4d13-b696-99331bce8644)
- 26, Attribute Set\
  ![Screenshot_20250226_131117_Samsung_capture](https://github.com/user-attachments/assets/1ce8b034-9a6d-44c7-8ed5-6287211c6b9d)\
  ![Screenshot_20250226_131141_Samsung_capture](https://github.com/user-attachments/assets/787d97e3-c566-4de7-a25c-139e8ad74afa)\
  1, In previous, We have created the gameplaytags, Gameplayability, and the next part is creating the attributeSet\
  2, In WarriorAttribute.h
  ```c++
  #include "CoreMinimal.h"
  #include "AttributeSet.h"
  #include "AbilitySystem/WarriorAbilitySystemComponent.h"
  #include "WarriorAttributeSet.generated.h"

  #define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
  GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
  GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
  GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

  /**
   * 
   */
  UCLASS()
  class WARRIOR_API UWarriorAttributeSet : public UAttributeSet
  {
	GENERATED_BODY()

  public:
	UWarriorAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentRage)

	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxRage)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, DefensePower)
  };
  ```
  3, In WarriorAttribute.cpp
  ```c++
  UWarriorAttributeSet::UWarriorAttributeSet()
  {
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
  }
  ```
  4, run it, play the game, press `, type `showdebug abilitysystem`\
  ![Screenshot 2025-02-26 133431](https://github.com/user-attachments/assets/42fe30da-6b41-4568-ab43-0f4ba234d5b3)
- 27, GameplayEffect\
  Purpose: Required to change attributes\
  1, Create a new blueprint based on GameplayEffect named GE_Hero_StartUp\
  2, Add the modifiers for each attribute that needs to be modified\
  3, Create a new Miscellaneous -> Curve Table, named `DT_HeroStatus`, for dynamic control modifier magnitute\
  4, Design the curve\
  ![Screenshot 2025-02-26 192813](https://github.com/user-attachments/assets/e8f759c0-60d8-406a-b15c-4b3c549871ab)\
  5, Attach those curves\
  ![Screenshot 2025-02-26 193046](https://github.com/user-attachments/assets/90e40efb-3791-42cb-8e9d-7036758c5b08)\
  6, Create a new blueprint based on GamePlayEffect named `GE_Hero_Status`\
  7, In this part, we must use `Attribute Based` for the Magnitude Calculation Type, add attribute to capture, and the attribute source.\
  ![Screenshot 2025-02-26 193452](https://github.com/user-attachments/assets/61b63f65-ced1-4c89-80e4-5eba71d842c8)\
  8, Just add maxhealth and maxrage.
- 28, Apply Gameplay Effect To Self\
  Purpose: apply those gameplay effects in the StartUp Data\
  1, In DataAssetStartUpDataBase.h
  ```c++
  
  UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
  TArray< TSubclassOf < UGameplayEffect > > StartUpGameplayEffects;
  ```
  2, in cpp
  ```c++
  if (!StartUpGameplayEffects.IsEmpty())
  {
	for (const TSubclassOf < UGameplayEffect >& EffectClass : StartUpGameplayEffects)
	{
		if (!EffectClass) continue;

		UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();

		InASCToGive->ApplyGameplayEffectToSelf(
			EffectCDO,
			ApplyLevel,
			InASCToGive->MakeEffectContext()
		);
	}
  }
  ```
  3, Add those GameplayEffect in DA_Hero\
  ![Screenshot 2025-02-26 195538](https://github.com/user-attachments/assets/2434604d-2293-4a95-80ab-55dfab1f0ef1)
- 29, Init Enemy Attributes\
  similar AS step 27, 28\
  1, use the command `showdebug abilitysystem`, and use the button page down\
  ![Screenshot 2025-02-27 093601](https://github.com/user-attachments/assets/694dfcf7-dc6e-4f80-97fd-05b3d4d4e112)\
  2, go to config/DefaultGame.ini\
  ```c++
  [/Script/GameplayAbilities.AbilitySystemGlobals]
  bUseDebugTargetFromHud = true
  ```
  ![Screenshot 2025-02-27 094417](https://github.com/user-attachments/assets/a447ef95-45a7-471c-a319-3e1888d764e6)
- 30, Pawn Combat Interface\
  Purpose: let BaseCharacter implement the Interface's pure virtual function `GetPawnCombatComponent()`, and its children: HeroCharacter and EnemyCharacter to overwrite this Interface\
  ![Screenshot_20250228_201129_Samsung_capture](https://github.com/user-attachments/assets/89e00ad7-b562-4ab1-aa19-54cbb888efda)\
  1, Create a new C++ based on unrealinterface named PawnCombatInterface\
  2, In PawnCombatInterface.h
  ```c++
  public:
	virtual UPawnCombatComponent* GetPawnCombatComponent() const = 0;
  ```
  3, Add this interface into WarriorBaseCharacter.h
  ```c++
  //~ Begin PawnCombatInterface Interface.
  virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
  //~ End PawnCombatInterface Interface
  ```
  Implement it in .cpp
  ```c++
  UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
  {
	return nullptr;
  }
  ```
  4, In WarriorHeroCharacter.h
  ```c++
  //~ Begin PawnCombatInterface Interface.
  virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
  //~ End PawnCombatInterface Interface
  ```
  In .cpp
  ```c++
  UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
  {
	return HeroCombatComponent;
  }
  ```
  5, In WarriorEnemyCharacter, do the same as WarriorHeroCharacter.\
  6, Create a blueprint callable function to detect whether the actor  is valid or not in WarriorFunctionLibrary\
  In .h
  ```c++
  static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);
  UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
  static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType);
  ```
  In .cpp
  ```c++
  UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
  {
	check(InActor);

	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
  }

  UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType)
  {
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;

	return CombatComponent;
  }
  ```
  7, For code quality, need to place all enum types in the same file\
  Create a new file named WarriorEnumTypes.h
  ![Screenshot 2025-02-28 203419](https://github.com/user-attachments/assets/285d2576-6e6a-49cf-b5a4-6de48aecb3c7)\
  8, In WarriorEnumTypes.h
  ```c++
  UENUM()
  enum class EWarriorConfirmType : uint8
  {
	Yes,
	No
  };

  UENUM()
  enum class EWarriorValidType : uint8
  {
	Valid,
	Invalid
  };
  ```
  9, Create a new AnimNotifyState named ANS_ToggleWeaponCombatCollision\
  ![Screenshot 2025-02-28 203819](https://github.com/user-attachments/assets/b24f6c35-c955-4025-986d-0243390cc3a7)\
  10, Add the new Notify\
  ![Screenshot 2025-02-28 203839](https://github.com/user-attachments/assets/5cc81738-4741-4ee9-a917-ca5a1e541d56)\
  11, test.\
- 31, Toggle Weapon Collison\
  Purpose: In the previous part, I clarified the combatcomponent owner. Here, we need to add a notification when should the collision start.\
  1, In PawnCombatComponent.h
  ```c++
  UENUM(BlueprintType)
  enum class EToggleDamageType : uint8
  {
	CurrentEquippedWeapon,
	LeftHand,
	RightHand
  };
  ....
  UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
  void ToggleWeaponCollision(bool bShouldEnable,EToggleDamageType ToggleDamageType = EToggleDamageType::CurrentEquippedWeapon);
  ```
  2, In .cpp
  ```c++
  void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
  {
	if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
	{
		AWarriorWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();

		check(WeaponToToggle);

		if (bShouldEnable)
		{
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Debug::Print(WeaponToToggle->GetName() + TEXT(" collision enabled"),FColor::Green);
		}
		else
		{
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Debug::Print(WeaponToToggle->GetName() + TEXT(" collision disabled"),FColor::Red);
		}		
	}
  }
  ```
  3, In ANS_ToggleWeaponCollision\
  ![Screenshot 2025-03-02 123406](https://github.com/user-attachments/assets/25b49f9c-b9aa-4542-bd6c-2aecf1e0a429)\
  ![Screenshot 2025-03-02 123450](https://github.com/user-attachments/assets/17afa450-3069-496c-901b-da2450d1ffa3)
- 32, OnWeaponBeginOverlapDone\
  Purpose: using a delegate to determine when collision overlap happened.\
  1, In WarriorWeaponBase.h, use `OnCollisionBoxBeginOverlap`, `OnCollisionBoxEndOverlap` to identify the beginning and end of collision overlap.
  ```c++
  UFUNCTION()
  virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
  virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
  ```
  2, In WarriorWeaponBase.cpp, use `GetInstigator<APawn>()` to retrieve the pawn that spawned or owns the weapon.
  ```C++
  void AWarriorWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
  {
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instiagtor as the owning pawn of the weapon: %s"), *GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn != HitPawn)
		{
			Debug::Print(GetName() + TEXT(" begin overlap with ") + HitPawn->GetName(), FColor::Green);
		}

		//TODO:Implement hit check for enemy characters
	}
  }

  void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
  {
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	checkf(WeaponOwningPawn, TEXT("Forgot to assign an instiagtor as the owning pawn of the weapon: %s"), *GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn != HitPawn)
		{
			Debug::Print(GetName() + TEXT(" end overlap with ") + HitPawn->GetName(), FColor::Red);
		}

		//TODO:Implement hit check for enemy characters
	}
  }
  ```
- 33, ShowUp Action Activities\
  Purpose: show up which actor gets hit, which weapon gets pulled, and who is the attacker.\
  1, In WarriorweaponBase.h, declare two variables `OnWeaponHitTarget`, `OnWeaponPulledFromTarget`, by using delegate for multi-call out.
  ```C++
  DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate,AActor*)
  ...
  FOnTargetInteractedDelegate OnWeaponHitTarget;
  FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
  ```
  2, In .cpp, excute the delegate.
  ```c++
  OnWeaponHitTarget.ExecuteIfBound(OtherActor);
  ```
  3, In PawnCombatComponent.h, enable these notification functions
  ```c++
  virtual void OnHitTargetActor(AActor* HitActor);
  virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);
  ```
  4, In HeroCombatCompoenet.h, override it\
  In .cpp
  ```c++
  void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
  {
	Debug::Print(GetOwningPawn()->GetActorNameOrLabel() + TEXT(" hit ") + HitActor->GetActorNameOrLabel(), FColor::Green);
  }

  void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
  {
	Debug::Print(GetOwningPawn()->GetActorNameOrLabel() + TEXT("'s weapon pulled from ") + InteractedActor->GetActorNameOrLabel(), FColor::Red);
  }
  ```
- 34, Notify Melee Hit\
  Purpose: In previous Steps, we can detect which one is the attacker, which one is being attacked, and the weapon types.\
  Here, we need to use a notify tag to enable the applied damage.\
  ![Screenshot_20250302_193451_Samsung_capture](https://github.com/user-attachments/assets/7d46f606-e0b3-4b41-bcce-9ad035d53954)\
  1, In WarriorGameplayTags.h
  ```c++
  /** Shared tags **/
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_MeleeHit);
  ```
  2, In .cpp
  ```c++
  /** Shared tags **/
  UE_DEFINE_GAMEPLAY_TAG(Shared_Event_MeleeHit,"Shared.Event.MeleeHit");
  ```
  3, In PawnCombatComponent.h, we need a Tarray to store the get-hit actor.
  ```c++
  protected:
	TArray<AActor*> OverlappedActors;
  ```
  4, In .cpp
  ```c++
  else
  {
	WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlappedActors.Empty();
  }
  ```
  5, In HeroCombatComponent.cpp, we need to detect if the get-hit actor exists, then push the gameplayevent.
  ```c++
  #include "AbilitySystemBlueprintLibrary.h"
  #include "WarriorGameplayTags.h"
  ...
  void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
  {
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Shared_Event_MeleeHit,
		Data
	);
  }
  ```
  6, In GA_lightAttackMaster\
  ![Screenshot 2025-03-02 200644](https://github.com/user-attachments/assets/d8801bd3-5ff3-45da-a50e-98a684e176f7)\
  ![Screenshot 2025-03-02 200700](https://github.com/user-attachments/assets/17016c04-13fd-4d98-98ee-ed4d0f232906)
- 35, finish all montages\
  1, set the `UsedComboCount` to private\
  2, fill all montage's notify state and finish the heavy attack blueprint\
  https://github.com/user-attachments/assets/132b6fa9-72db-4ec0-85f9-f8565c935d85
- 36, HeroDamageInfoDone\
  ![Image_1741020850349](https://github.com/user-attachments/assets/b437d010-7068-4278-96be-253b63577d7f)\
  Purpose: create a damaging effect that will be applied to targets in the game. \
  It sets up all the necessary context (who's dealing the damage, what ability is being used, etc.) \
  and configures the damage parameters (base damage, attack type, combo count). \
  1, In WarriorHeroGameplayAbility.h
  ```c++
  UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
  FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass,float InWeaponBaseDamage,FGameplayTag InCurrentAttackTypeTag,int32 InCurrentComboCount);
  ```
  2, In.cpp, In here, we need to create two FGameplayEffectContextHandle, `ContextHandle` and `EffectSpecHandle`\
  The first one provides all the necessary information about "who" and "how" an effect is being applied :\
  Who is causing the effect (the instigator)\
  What ability is applying the effect\
  What actor is the source of the effect\
  Any additional source objects\
  The second one combines the context with the actual effect class and details\
  Which gameplay effect class to use\
  What level should the effect be applied at\
  What the magnitudes of the effect should be\
  The context (it contains the ContextHandle you created earlier)
  ```c++
  FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InCurrentComboCount)
  {	
	check(EffectClass);

	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);

	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag,InCurrentComboCount);
	}

	return EffectSpecHandle;
  }
  ```
  3, In WarriorGamplayTags.h
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_SetByCaller_BaseDamage);
  ```
  4, In .cpp
  ```c++
  UE_DEFINE_GAMEPLAY_TAG(Shared_SetByCaller_BaseDamage,"Shared.SetByCaller.BaseDamage");
  ```
  5, In GA_LightAttackMaster, create this new blueprint `Make Outgoing Gameplay Effect Spec`
  ![Screenshot 2025-03-04 090844](https://github.com/user-attachments/assets/7c449d89-b8f5-424a-86cf-857397ecb59b)
- 37, HeroDamageInfoDone - HeroDamage\
  Purpose: In 36, We created a new blueprint `Make Outgoing Gameplay Effect Spec`, in this part, we have to fill up \
  EffectClass, InWeaponBaseDamage, InCurrentTypeTag, InCurrentComboCount\
  1, Create a new gameplayEffect named `GE_Shared_DealDamage`\
  2, Create a new calculation class to fill in Excusion -> calculation class, so that, create a new C++ based on gameplayeffectexecution name 
  3,![Screenshot 2025-03-04 090739](https://github.com/user-attachments/assets/9d5ce056-a48c-461f-bcad-0c2867d623f5)\
     ![Screenshot 2025-03-04 091000](https://github.com/user-attachments/assets/ab2a62a5-47b2-4a63-b9ad-1e1d7aa64f41)\
  4, In WarriorStructTypes.h, we need to add `WeaponBaseDamage` in FWarriorHeroWeaponData
  ```c++
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FScalableFloat WeaponBaseDamage;
  ```
  5, In HeroCombatComponent.h
  ```c++
  UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
  AWarriorHeroWeapon* GetHeroCurrentEquippedWeapon() const;

  UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
  float GetHeroCurrentEquippWeaponDamageAtLevel(float InLevel) const;
  ```
  6, In .cpp
  ```c++
  AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
  {
	return Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
  }

  float UHeroCombatComponent::GetHeroCurrentEquippWeaponDamageAtLevel(float InLevel) const
  {
	return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
  }
  ```
  7, In BP_HeroAxe, search for the hero, and create a new data curve table to store weapon damage by different levels\
  ![Screenshot 2025-03-04 092609](https://github.com/user-attachments/assets/9bf13529-05d7-4faf-9f3d-cf2e45e83041)\
  8, Create a new Curve Table named CT_HeroWeaponStats and attach it into BP_HeroAxe\
  9,![Screenshot 2025-03-04 093324](https://github.com/user-attachments/assets/1025d515-5238-4182-8318-041b54583edd)\
  10, Create the attack type tag, in WarriorGameplayTags.h
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Light);
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Heavy);
  ```
  11, fill in the .cpp
  12,![Screenshot 2025-03-04 094151](https://github.com/user-attachments/assets/edf6a816-bdba-4206-8a3f-ecb31ed6014d)
- 38, Applied the `Make Hero Damage Effect Spec Handle` To Target\
  Purpose: Applied this handle to target hero\
  1, In WarriorEnumTypes.h
  ```c++
  UENUM()
  enum class EWarriorSuccessType : uint8
  {
	Successful,
	Failed
  };
  ```
  2, In WarriorGameplayAbility.h, create a new function NativeApplyEffectSpecHandleToTarget to handle this action.
  ```c++
  FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle);

  UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
  FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle,EWarriorSuccessType& OutSuccessType);
  ```
  3, In .cpp, using `ApplyGameplayEffectSpecToTarget()` in Abilitysystemcomponent to do that.
  ```c++
  FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
  {	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	check(TargetASC && InSpecHandle.IsValid());

	return GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
  }

  FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType)
  {
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor,InSpecHandle);

	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied()? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;

	return ActiveGameplayEffectHandle;
  }
  ```
  4, link the blueprint\
  ![Screenshot 2025-03-05 090946](https://github.com/user-attachments/assets/f3ef915b-37be-4491-ae7b-2a5bf385c1fd)\
- 39, Gameplay Effect Execution Calculation - Capture attributes for calculation
  Purpose: Capture which attributes for calculation\
  1, In GEExecuteCal_DamageTaken.h
  ```c++
  public:
	
	UGEExecuteCal_DamageTaken();
  ```
  2, In .cpp
  ```c++
  struct FWarriorDamageCapture
  {
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)

	FWarriorDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false)
	}
  };

  static const FWarriorDamageCapture& GetWarriorDamageCapture()
  {
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
  }

  UGEExecuteCal_DamageTaken::UGEExecuteCal_DamageTaken()
  {
	/*Slow way of doing capture*/
	//FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
	//	UWarriorAttributeSet::StaticClass(),
	//	GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet,AttackPower)
	//);

	//FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
	//	AttackPowerProperty,
	//	EGameplayEffectAttributeCaptureSource::Source,
	//	false
	//);

	//RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);

	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
  }
  ```
- 40, Retrieve Hero Damage Info\
  Purpose: using `Execute_Implementation`allows gameplay effects to calculate results based on attributes captured in 39\
  1, GEExecuteCal_DamageTaken.h
  ```c++
  virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
  ```
  2, In .cpp\
  `const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes` checks the data curve table.\
  ```c++
  const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

  /*EffectSpec.GetContext().GetSourceObject();
  EffectSpec.GetContext().GetAbility();
  EffectSpec.GetContext().GetInstigator();
  EffectSpec.GetContext().GetEffectCauser();*/

  FAggregatorEvaluateParameters EvaluateParameters;
  EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
  EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

  float SourceAttackPower = 0.f;
  ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParameters, SourceAttackPower);

  float BaseDamage = 0.f;
  int32 UsedLightAttckComboCount = 0;
  int32 UsedHeavyAttackComboCount = 0;

  for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
  {
	if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
	{
		BaseDamage = TagMagnitude.Value;
	}

	if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
	{
		UsedLightAttckComboCount = TagMagnitude.Value;
	}

	if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
	{
		UsedHeavyAttackComboCount = TagMagnitude.Value;
	}
  }

  float TargetDefensePower = 0.f;
  ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef, EvaluateParameters, TargetDefensePower);
  ```
- 41, CalculateFinalDamageDone\
  Purpose: In 39, 40, we capture the required attributes, and get the required data from the data table based on gameplayeffecttag. this step we need to calc the DamageTaken\
  1, In warriorattributrset.h
  ```c++
  UPROPERTY(BlueprintReadOnly, Category = "Damage")
  FGameplayAttributeData DamageTaken;
  ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,DamageTaken)
  ```
  2, In DebugHelper.h, we need to see the final damagetaken\
  ```c++
  static void Print(const FString& FloatTitle, float FloatValueToPrint, int32 InKey = -1, const FColor& Color = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			const FString FinalMsg = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);

			GEngine->AddOnScreenDebugMessage(InKey,7.f,Color,FinalMsg);

			UE_LOG(LogTemp,Warning,TEXT("%s"),*FinalMsg);
		}
	}
  ```
  3, In GEExecuteCal_DamageTaken.cpp
  ```c++
  Debug::Print(TEXT("TargetDefensePower"),TargetDefensePower);

	if (UsedLightAttckComboCount != 0)
	{
		const float DamageIncreasePercentLight = (UsedLightAttckComboCount - 1) * 0.05 + 1.f;

		BaseDamage *= DamageIncreasePercentLight;
		Debug::Print(TEXT("ScaledBaseDamageLight"),BaseDamage);
	}

	if (UsedHeavyAttackComboCount != 0)
	{
		const float DamageIncreasePercentHeavy = UsedHeavyAttackComboCount * 0.15f + 1.f;

		BaseDamage *= DamageIncreasePercentHeavy;
		Debug::Print(TEXT("ScaledBaseDamageHeavy"),BaseDamage);
	}

	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;
	Debug::Print(TEXT("FinalDamageDone"),FinalDamageDone);

	if (FinalDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetWarriorDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamageDone
			)
		);
	}
  ```
- 42, Set Up Heavy Attack Blueprint\
  ![Screenshot 2025-03-05 190938](https://github.com/user-attachments/assets/a8107401-8cd0-45ca-aafd-d03d15af38ce)\
- 43, Modify Health Attribute\
  Purpose: Using the FinalDamage in the Health Attribute.\
  1,  In warriorAttributeSets.h
  ```c++
  protected:
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
  ```
  2, In .cpp
  ```c++
  void UWarriorAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
  {
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);
	}

	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());

		SetCurrentRage(NewCurrentRage);
	}

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();

		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);

		const FString DebugString = FString::Printf(
			TEXT("Old Health: %f, Damage Done: %f, NewCurrentHealth: %f"),
			OldHealth,
			DamageDone,
			NewCurrentHealth
		);

		Debug::Print(DebugString, FColor::Green);

		//TODO::Notify the UI 

		//TODO::Handle character death
		if (NewCurrentHealth == 0.f)
		{

		}
	}
    }
  ```
- 44, Enemy HitReact ability done\
  Purpose: Create enemy Ability Tags, gameplay ability blueprint, anim montage, grant ability\
  1, In WarriorGameplayTags.h
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Melee);
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Ranged);
  ...
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_HitReact);
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_HitReact);
  ```
  2, fill the .cpp\
  3, Create a new gameplay blueprint based on warriorenemygameplayability named `GA_Enemy_HitReact_Base`\
  4,![Screenshot 2025-03-07 085425](https://github.com/user-attachments/assets/489bbcac-f3aa-4c52-9c54-2c27b3172436)\
  5,![Screenshot 2025-03-07 085450](https://github.com/user-attachments/assets/473fd844-d412-490c-9910-10d9b09651ca)\
  6,![Screenshot 2025-03-07 090149](https://github.com/user-attachments/assets/69d59914-f7ce-431d-bcfa-a93794445304)\
  7, create a new child of GA_Enemy_HitReact_Base, named `GA_Guadrain_HitReact`\
  8, create two new anim montages and attach them in GA_Guadrain_HitReact\
  ![Screenshot 2025-03-07 090128](https://github.com/user-attachments/assets/204066ac-8156-4cc1-b509-bad18972e8e0)\
  9, In DA_Guardian\
  ![Screenshot 2025-03-07 090254](https://github.com/user-attachments/assets/8f283c54-d852-4f53-9908-49a2f0ef483d)
- 45, Trigger Hit React Ability\
  ![Screenshot_20250309_111417_Samsung_capture](https://github.com/user-attachments/assets/c49b05de-a738-419c-9139-788bc77f7c6c)\
  Purpose: Enable the hot react action\
  1, In GA_LightAttack, we need to send the gameplay event to the actor, and this gameplay event is stored in gameplay event data(the actor is being hit)\
  2, In GA_HitReact_Base, we need to enable the enemy to automatically face the instigator when it gets hit.\
  ![Screenshot 2025-03-09 112513](https://github.com/user-attachments/assets/f422cab1-7c35-4ed9-ba7d-4ff9e2190969)\
  ![Screenshot 2025-03-09 112437](https://github.com/user-attachments/assets/3bb6e863-2200-4473-9310-aadc38b33010)
- 46, Hit Fix -Material Hit FX\
  Purpose: enable the hit material FX effect when getting hit\
  1, find the material of the enemy\
  ![Screenshot 2025-03-09 115851](https://github.com/user-attachments/assets/a92b7633-eda4-406f-869e-75cbd42ce14f)\
  2, Create material instance\
  ![Screenshot 2025-03-09 120002](https://github.com/user-attachments/assets/f2be06d1-f8ba-4b41-9111-499a2466ff8d)\
  3， Design the color and add it to the skeleton\
  ![Screenshot 2025-03-09 122123](https://github.com/user-attachments/assets/7cba818a-7ec0-44f9-a9e1-fb48144a0236)\
  4, Add the material color change in GA_HitReact\
  ![Screenshot 2025-03-09 122734](https://github.com/user-attachments/assets/b2762753-e3c2-4a52-847d-9d7df2bd3a8b)\
  ![Screenshot 2025-03-09 122724](https://github.com/user-attachments/assets/371797e1-1173-4f01-b609-5b8a82549c1a)
- 47, Hit Fix - Hit Pause\
  Purpose: Add a hit pause to enhance hit react feeling.\
  1, In WarriorGameplayTags.h
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_HitPause);
  ...
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_HitPause);
  ```
  2, fill the .cpp\
  3, Create a new gameplay ability blueprint named `GA_Hero_Hitpasuse`\
  4, Change the class seetings.\
  ![Screenshot 2025-03-09 180418](https://github.com/user-attachments/assets/f04e1972-793a-4426-8364-b81843394716)\
  ![Screenshot 2025-03-09 180423](https://github.com/user-attachments/assets/e2b3679a-4e31-4f95-bda1-13faae0cd2c6)\
  ![Screenshot 2025-03-09 180428](https://github.com/user-attachments/assets/bb17c3d7-4aee-42fa-9a57-726dd7f03a06)\
  5, ![Screenshot 2025-03-09 180653](https://github.com/user-attachments/assets/9c022533-4f1e-4e40-b9c4-045d040c0501)\
  6, Add it to the DA_Hero\
  ![Screenshot 2025-03-09 180748](https://github.com/user-attachments/assets/bc5cc43a-a5b1-45a2-aa6f-498f8ede2102)\
  7, In HeroCombatComponent.cpp
  ```c++
  UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);
  ...
  void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
  {
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Player_Event_HitPause,
		FGameplayEventData()
	);
  }
  ```
- 48, Camera Shake\
  Puropse: add camera shake by using blueprint\
  1, create a new camershake blueprint named `Camerashake_HeroMelle`
  2, In GA_Hero_Hitpasuse, add this camerashake blueprint\
  ![Screenshot 2025-03-09 182844](https://github.com/user-attachments/assets/91a87182-1a4e-4e17-aeba-a3f10e80e9b5)\
  3, customize your camera shake.
- 49, Sound FX - Hit React Sound\
  Purpose: Enable the the hit React Sound play and use courrency to allow only one sound to play.\
  1, Give a new notify sound effect into the AM_enemy\
  2, Create a new blueprint concurrency named `Concurrency_OneAtATime`\
  ![Screenshot 2025-03-09 194301](https://github.com/user-attachments/assets/46d572dc-76b9-489d-ba28-0b649f14ef33)\
  3, Find that attach sound , attach the concurrency \
  ![Screenshot 2025-03-09 194514](https://github.com/user-attachments/assets/83b4ae54-560d-4c23-a525-67c097ab17cc)\
- 50, Sound FX - Melee Hit Sound\
  Purpose: In this situtation, we can not use add notify to play this sound, because it only happened when we hit somethings.\
  So, we need to use `GameplayCues` to play that sound when hit this action happened.\
  1, Create a new folder named `GameplayCues` under folder Content\
  2, Create a new buleprint based on GameplycueNotify_Static named `GC_Hero_AxeHit`\
  3, In GameplayCueTag, create a new gameplayTag.\
  ![Screenshot 2025-03-09 200709](https://github.com/user-attachments/assets/c210d7a8-2e26-4cbe-b2fb-adaa1f681c35)\
  4, In GC_Hero_AxeHit, create a new execution blueprint.\
  ![Screenshot 2025-03-09 201108](https://github.com/user-attachments/assets/06845515-57a0-434e-b114-f214fce9009e)\
  5, In GA_Hero_LightAttackMaster,\
  ![Screenshot 2025-03-09 201258](https://github.com/user-attachments/assets/78f3f37a-1edf-4a4e-8cb4-0b4f92934e7a)\
  6, In Config/DefaultGame.ini
  ```c++
  GameplayCueNotifyPaths = "/Game/GameplayCues"
  ```
  7, Add the execute gamplaycue on Owner, let the WeaponSoundGameplayCueTag to be null(same as light attack)\
  ![Screenshot 2025-03-09 202228](https://github.com/user-attachments/assets/3c7d5689-6784-4afe-8198-cfafd2ab5a1a)\
  8, Then change all child buleprint of heavy/light attack, make sure sure their WeaponSoundGameplayCueTag is gameplaycue.meleehit.axehit
- 51, Enemy Death Ability\
  Purpose: Add a tag to the actor, and use it to trigger the play death montage.\
  1, In WarriorTags.h
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_Death);
  ...
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Dead);
  ```
  2, Do in the .cpp\
  3, In WarriorAttributeSet.h
  ```c++
  if (NewCurrentHealth == 0.f)
	{	
		UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGameplayTags::Shared_Status_Dead);
			
	}
  ```
  4, create a new blueprint based on Gameplay Blueprint, named GA_Enemy_Death_Base\
  5, ![Screenshot 2025-03-10 191009](https://github.com/user-attachments/assets/1a1ef7ba-3793-454a-ac58-a85d44c8ffed)\
  ![Screenshot 2025-03-10 191012](https://github.com/user-attachments/assets/ffebaf7d-6aa8-4853-a6c0-84246a7622c8)\
  ![Screenshot 2025-03-10 191015](https://github.com/user-attachments/assets/9261381e-e357-4cb7-9ad5-61fbf6e28ca9)\
  6, Create a new child class named GA_Guadrain_Death\
  7, In DA_Guadrain\
  ![Screenshot 2025-03-10 191246](https://github.com/user-attachments/assets/632c7836-3c99-4817-a5f2-84886498b195)\
  8, Create a new gameplay sound blueprint named `GC_Guadrain_DeathSound`\
  9,![Screenshot 2025-03-10 192131](https://github.com/user-attachments/assets/2fd585b8-a061-401f-ba6c-c85fd33eadea)\
  ![Screenshot 2025-03-10 192302](https://github.com/user-attachments/assets/43beb726-5005-4154-9f73-4425abc17261)\
  10, In GA_Guadrain_Death, add the new tag to sound play\
  ![Screenshot 2025-03-10 192412](https://github.com/user-attachments/assets/f575723d-e72c-404e-9b5c-1a1280bb034b)
  11, make sure all animation's enablemotion is checked.\
  ![Screenshot 2025-03-10 192543](https://github.com/user-attachments/assets/11bdfe32-4ced-49b5-9fc4-9eb564539a39)\
  ![Screenshot 2025-03-10 192638](https://github.com/user-attachments/assets/c311e87e-8622-4bce-ae53-ecabaa9bcddb)\
  12, After creating the animation montage, add them all\
  ![Screenshot 2025-03-10 192847](https://github.com/user-attachments/assets/42c20146-1b91-4a43-b65f-b0c808e5cd88)\
- 52, BP Death Interface\
  Purpose: Instead of using cast to, use an interface to enable some specific function.\
  1,

- 53, DIssolve Material FX\
  Purpose: enable the dissolved material to disappear.\
  1, In BP_Warriorenemy_base, create a new timeline, using a set play rate to decide the display speed.\
   ![Screenshot 2025-03-13 195937](https://github.com/user-attachments/assets/c64a60f3-d567-4d44-8aa8-8fc387e4263b)\
  2, for weapon, go to its color material,\
  ![Screenshot 2025-03-13 200546](https://github.com/user-attachments/assets/2ca888b3-c03a-4503-b148-2b8fc260f994)\
  3, create a material instance, named `MI_GuadrainWeapon`, go to static mesh of BP_Guadrainweapon, and change its material slot to the new material instance.\
  4, copy all color data from MI_guardian to the MI_GuadrainWeapon, change the bp_weapon material, enable the dissolve in mi_guardain.\
  5, in warriorgameplayenemybase,
  ![Screenshot 2025-03-13 204353](https://github.com/user-attachments/assets/d203b02e-24b8-49ed-96b1-37d1ec26a687)
- 54, Dissolve Niagaraga FX\
  Purpose: enable the dissolved Niagara to appear.\
  1, in BPI_Enemy_Death, create new inputs: DissolveNiagaraSystem, type is soft reference, Niagara system\
  2, in GA_Enemy_Death_Base, \
  ![Screenshot 2025-03-14 102005](https://github.com/user-attachments/assets/eecdd20e-9345-4b25-8742-c1b27c99e109)\
  3, in GA_Guadrain_Death, attach the new Niagara FX,\
  ![Screenshot 2025-03-14 102347](https://github.com/user-attachments/assets/417cfeab-dac0-4e49-8e93-f21270567b19)\
  4, in BP_WarriorEnemyCharacter_Base, using soft reference, dynamic get enemy color, change niagara FX color\
  ![Screenshot 2025-03-14 105340](https://github.com/user-attachments/assets/a6232f0e-d779-4956-847d-4ea142730139)\
  ![Screenshot 2025-03-14 105348](https://github.com/user-attachments/assets/0f9d0302-ece1-4f38-aae9-aeac710a4d6f)\
  ![Screenshot 2025-03-14 105353](https://github.com/user-attachments/assets/ee410259-670c-471a-bc1a-cc8f0ca400bd)
- 55, Pawn UI Interface\
  Purpose: Using an interface to accept attribute change (Hero/Enemy)\
  1, Create a new child PawnUIcomponent based on PawnExtensionComponent\
  2, Create hero, and enemy ui components based on  PawnUIcomponent\
  3, Create a new unreal interface, PawnUIInterface\
  4, add those interfaces, component into the files.\
- 56, BroadCast Value Change\
  Purpose: 1, using `DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam` enable multi call back specific value\
  2, using `boardcast()` allows call back the delegate\
  3, using `weak_ptr` to use less memory.\
  [`Code`](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/c6d4c04163d4cad9b754951abb0278bcf21f6c5e
)
- 57, Listen For BoardCast - Create a hero UI widget\
  Purpose: Create a new widget base, using it to enable processing the uiinterface's uicomponents\
  [`code`](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/6e9f7c381170ec3c3cefe4fc09fa901e17ca0f36)\
  1, create a new C++ based on the userwidget, named `WarriorWidgetBase`, and make the font size bigger\
  2, ![Screenshot 2025-03-16 140228](https://github.com/user-attachments/assets/93c1ab9b-84c2-49b4-941e-8e981dbc8df6)\
  3, In BP_WarriorPlayerCharacter,\
  ![Screenshot 2025-03-16 140507](https://github.com/user-attachments/assets/bef57cb1-7410-49f4-be8b-6223aa88b5f8)
- 58, Enemy Init Created Widget\
  Purpose: Allow widget created for enemy\
  [`code`](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/41f69f11907011872ca6dfca7d187786f0af0f81)\
  1, NativeOnInitialized() doesn’t crash because missing HeroUIComponent is okay, but InitEnemyCreatedWidget() expects EnemyUIComponent to always exist, so it forces a crash when missing.\
  2, so For the hero ui component, it can be shown or not shown; but for the enemy ui component, it should always be shown.
  3, In WBP_Test,\
  ![Screenshot 2025-03-16 174736](https://github.com/user-attachments/assets/309a46c3-99af-4651-b8fc-b3f82e4b2d7c)\
  4, In BP_WarriorEnemy_Base,\
  ![Screenshot 2025-03-16 175104](https://github.com/user-attachments/assets/3c681e67-48e1-49a1-a773-8f6f33e05016)
- 59, Template Widgets\
  Purpose: create the health bar\
  1, ![Screenshot 2025-03-16 183439](https://github.com/user-attachments/assets/2d156e61-7fa6-46b8-93d9-a015c3d64696)\
  2, ![Screenshot 2025-03-16 183525](https://github.com/user-attachments/assets/61bbaa60-d073-46e9-9fab-49513eb5111c)\
- 60, Set Status Bar Fill Color\
  Purpose: enable different health percent show different colors\
  1, ![Screenshot 2025-03-16 190759](https://github.com/user-attachments/assets/46ae8d75-5edd-474c-ad05-4136369483fe)\
  2, ![Screenshot 2025-03-16 190804](https://github.com/user-attachments/assets/1743be5d-418f-4162-835f-a0f3c7867eaf)\
  3, ![Screenshot 2025-03-16 190828](https://github.com/user-attachments/assets/1094753f-9779-4daf-9205-3b41dc35f191)
- 61, Template icon slot widget\
  Purpose: create an icon slot widget.\
  1, ![Screenshot 2025-03-17 094648](https://github.com/user-attachments/assets/e7a9fa31-0bc7-41a6-b507-e94ddc55b1a3)\
  2, ![Screenshot 2025-03-17 094710](https://github.com/user-attachments/assets/f38f9bb5-0288-42c4-a865-2aaa804e72d7)\
  3, ![Screenshot 2025-03-17 094719](https://github.com/user-attachments/assets/e32dc902-3c3b-43d7-b738-23b0d589124d)
- 62, Hero Overlay Widget\
  Purpose: create a hero overlay widget.\
  1, ![Screenshot 2025-03-18 095650](https://github.com/user-attachments/assets/075e5280-3b22-4595-a7af-c66d603a0d9d)\
  2, ![Screenshot 2025-03-18 095659](https://github.com/user-attachments/assets/6671c161-9564-42a9-a999-1bc88e45522a)\
  3, Create a new gameplayability, choose its gameplay ability to on Given, and add it on DA_Hero's ON Given.\
  4, ![Screenshot 2025-03-18 095921](https://github.com/user-attachments/assets/26876aa5-949c-4610-9b9f-914e8cce5ddf)\
  5, change the value to test overlay in GameplayEffect/GE_Hero_Static.\
  ![Screenshot 2025-03-18 100033](https://github.com/user-attachments/assets/d339e864-2d24-4cd2-8c2d-7b07583213ae)\
  ![Screenshot 2025-03-18 100038](https://github.com/user-attachments/assets/e01da1f9-cc57-4147-b1c0-c4ade428f891)
- 63, 


  











  


































  




  

























  
  


  










  


  






