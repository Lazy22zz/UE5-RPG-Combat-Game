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
  In step 5, we created a new input component, then we need to attach it to warriorherocharacter. If we do so, we need `SetupPlayerInputComponent` in Character.h
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
