 Creating a combat action RPG game

 # Table of Contents

- [UE5-RPG-Combat-Game (Using GAS, Gameplay Ability System)](#ue5-rpg-combat-game-using-gas-gameplay-ability-system)
- [EXTRA](#extra)
  - [1, Hard Reference](#1-hard-reference)
  - [2, Soft Reference](#2-soft-reference)
  - [3, Conclusion](#3-conclusion)
  - [4, TObjectPtr](#4-tobjectptr)
  - [5, TSubclassof](#5-tsubclassof)
  - [6, Synchronous and Asynchronous Loading](#6-synchronous-and-asynchronous-loading)
  - [7, Basic GameplayEffectHandle Code](#7-basic-gameplayeffecthandle-code)
  - [8, Automatic Storage, Static Storage, Dynamic Storage](#8-automatic-storage-static-storage-dynamic-storage)
  - [9, dynamic_cast, shared_ptr, weak_ptr, unique_ptr](#9-dynamic_cast-shared_ptr-weak_ptr-unique_ptr)
  - [10, checkf()](#10-checkf)
  - [11, Pre-construction and Construction](#11-pre-construction-and-construction)
  - [12, New Ability Process](#12-new-ability-process)
  - [13, World Of the UE](#13-World-Of-the-UE)
  - [14, Lamda Function](#14-Lamda-Function)
  - [15, Gameplay Ability System](#15-Gameplay-Ability-System)
  - [16, Pawn](#16-Pawn)
- [1. Set Up Hero Character](#1-set-up-hero-character)
- [2. Combo System](#2-combo-system)
- [3. Hero Combat](#3-hero-combat)
- [4. Enemy AI](#4-enemy-ai)
- [5. Hero Combat Ability](#5-hero-combat-ability)
- [6, Ranged Enemy](#6-Ranged-Enemy)

# EXTRA
## 1, Hard reference
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
## 2, Soft reference
  A soft reference is an delay pointer (via a FSoftObjectPath or TSoftObjectPtr) to another object or asset. It doesn't load the referenced object into memory until fully loaded. \
  1. FSoftObjectPath\
     A simple string path to an asset\
     Works like a "bookmark" to the asset without loading it\
     Shows up in the editor like a regular asset selector
  2. TSoftObjectPtr<Type>\
     A template version that restricts to specific types (like TSoftObjectPtr<UTexture2D>)\
     Slightly more powerful than FSoftObjectPath\
     Can automatically convert to the real object if it's already loaded
## 3, Conclusion
  ![Screenshot 2025-01-07 100832](https://github.com/user-attachments/assets/4895bb3b-7242-45c5-a380-8f636d9ba03f)
## 4, TObjectPtr
  "T" in UE5 means the Type, such as TArray, TMap; \
  TObjectPtr is *hard* reference in UObject pointer; \
  ![Screenshot 2025-01-07 103243](https://github.com/user-attachments/assets/03a5a1ba-d070-4fa8-b3f8-6c3645a17bc0)
## 5, TSubclassof
  Wrap up the required class type\
  Example:
  ```c++
  TArray< TSubclassOf < UWarriorGameplayAbility > > ActivateOnGivenAbilities;	
  ```
  We wrap up any types under `UWarriorGameplayAbility` that satisfy the requirements of `TArray<UClass *>`
## 6, Synchronous and Asynchronous loading
  In Unreal Engine C++, synchronous loading uses `LoadObject()` to load assets immediately, ensuring they are available before gameplay starts, while asynchronous loading uses `FStreamableManager::RequestAsyncLoad()` to load assets in the background without 
  blocking the main thread. Synchronous loading is ideal for critical assets like the player's character, ensuring smooth startup. In contrast, asynchronous loading is best for non-critical assets like enemies, optimizing performance and reducing initial load times.\
## 7, Basic GameplayEffectHandle code
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

  // Then, eventually, you apply the effect
  AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(EffectSpecHandle, TargetASC);
  ```
## 8, Automatic Storage, Static Storage, Dynamic Storage
  Automatic storage: variables defined inside a function use `automatic storage`, which means the variables exist automatically when the function containing them is invoked and expire when the function is terminated\
  Static Storage: exists throughout the execution of an entire program. Two ways to make a variable static: 1, define it externally, outside a function; 2, use the keyword `static` when declaring a variable\
  Dynamic Storage: Do dynamic data work.\
  +-------------------+  <- High memory address  \
  | Static / Global  |  (Global & static variables)  \
  +-------------------+  \
  |      Heap        |  (Grows upwards with `new` / `malloc()`)  \
  +-------------------+  \
  |      Stack       |  (Grows downwards with function calls)  \
  +-------------------+  <- Low memory address  \

## 9, `dynamic_cast`, `shared_ptr`, `weak_ptr`, `unique_ptr`
  ✅ Use dynamic_cast only to check and convert polymorphic types at runtime safely.\
  ✅ Use shared_ptr when you need multiple pointers for the same object.\
  ```
  #include <memory>

  std::shared_ptr<int> a = std::make_shared<int>(10);
  std::shared_ptr<int> b = a; // both a and b share ownership of the same int

  ```
  ✅ Use weak_ptr Works with shared_ptr only, and break the circular references.(cyclic references)\
  ```
  #include <memory>

  std::shared_ptr<int> shared = std::make_shared<int>(42);
  std::weak_ptr<int> weak = shared;

  if (auto locked = weak.lock()) {
    // locked is a shared_ptr
    std::cout << *locked << std::endl;
  }
  ```
  ✅ Use unique_ptr when owning the object at a time, and can not be copied.\
  ```
  #include <memory>

  std::unique_ptr<int> ptr = std::make_unique<int>(10);
  // std::unique_ptr<int> ptr2 = ptr; // ❌ compile error
  std::unique_ptr<int> ptr2 = std::move(ptr); // ✅ ownership transferred
  ```
## 9.1, memory leak, smart pointer
  📦 Memory leak: When a program allocates memory but fails to release it, even though it’s no longer needed. \
  📦 Smart Pointer: using RAII (Resource Acquisition Is Initialization). That means memory is automatically released when the pointer goes out of scope.[You can think it has built-in new, delete functions in the references.]\
  
## 10, `checkf()`
  Purpose: The game crashes and generates a report when it is false.\
  
## 11, pre-construction and construction 
  Pre-Construct → Runs before the widget is fully created and can be previewed in the editor.\
  Construct → Runs after the widget is created and is used for regular initialization at runtime.

## 12, New Ability Process
  1. Create Ability Tags.
  2. Create Ability Blueprint.
  3. Anim Montage.
  4. Ability Input Action.
  5. Grant Ability.

## 13, World Of the UE  
  1. UObject: Mother of the World, which is under the class.  
     ![UObject](https://github.com/user-attachments/assets/a1098c12-d867-475d-a3b7-8643eb6b0e7f)  
  2. AActor: Is birth from UObject.  
     Its Major functions: `Replication`, `Spawn`, `Tick`  
     Its Major types: `staticMeshActor`, `CameraActor`, `PlayerStarterActor`  
     Think: Why Doesn't Actor Have a Built-in Transform Like Unity's GameObject?  
     <details>
       <summary>Click to reveal</summary>
      To make development easier, Unreal Engine provides convenient methods like GetActorLocation() and SetActorLocation(), which internally delegate to the RootComponent. In the same way, the ability of an Actor to receive and process input events is actually forwarded to the internal UInputComponent* InputComponent; facilitation is also provided to access it directly through the Actor. Why this design? Because UE follows a C++ philosophy: "Never pay for what you don't need." In Unreal’s view, an Actor isn’t just a visible or physical object in the 3D world. Some Actors are completely invisible and serve only to represent information or control logic—like AInfo and its derived classes (AWorldSettings, AGameMode, AGameSession, APlayerState, AGameState, etc.), as well as AHUD and APlayerCameraManager. These Actors embody various rules, states, and systems that govern the game world. 
     </details>
  3. Component: Actor's skills\
     ![ActorAndComponent](https://github.com/user-attachments/assets/f934ca7c-4772-4c00-8112-c756976eaccc)\
     `TSet<UActorComponent*> OwnedComponents` holds all the Components owned by the Actor, usually one of which will be a SceneComponent as a RootComponent.\
     `TArray<UActorComponent*> InstanceComponents` holds instantiated Components. What does instantiation mean is the Component that you define in the Details in Blueprint, and when the Actor is instantiated, these attached Components will also be instantiated.
     For an Actor to be able to be placed in a Level, it must instantiate `USceneComponent* RootComponent`\
     `SceneComponents` provide two major capabilities: Transforms and nesting SceneComponents\
     ![Components](https://github.com/user-attachments/assets/4381032e-f880-4016-b757-4d21a487bcc5)\
     Major Components: `UActorComponent`, `USceneComponent`, `UPrimitiveComponent`\
     Think: Why can't ActorComponents nest within each other? And nesting is only available at the SceneComponent level?
     <details>
       <summary>Click to reveal</summary>
      First, an Actor doesn't just have a single USceneComponent—it can also include components like UMovementComponent, AIComponent, and any custom components we design. Second, when it comes to implementing game logic, Unreal Engine generally advises against placing core logic directly in Components.
     </details>
  4. Level: In UE5, a Level is like a country—think of it as China, the USA, or Russia. It's an `AActor` derived from `UObject`, specifically represented by the `ALevelScriptActor`, which defines the rules for that level. Each level has its settings, such as gravity, game mode, and lighting, which are managed through its `WorldSettings`. Among all levels, the `Persistent Level` acts as the primary one, while the others are treated as Sublevels.  
![ActorAndComponent](https://github.com/user-attachments/assets/87758371-e814-47bb-ab86-ffedb94798af)
  <details>
  <summary> View Code</summary>

  ```c++
  void ULevel::SortActorList()
  {
    if (Actors.Num() == 0)
    {
        // No need to sort an empty list
        return;
    }

    TArray<AActor*> NewActors;
    TArray<AActor*> NewNetActors;
    NewActors.Reserve(Actors.Num());
    NewNetActors.Reserve(Actors.Num());

    check(WorldSettings);

    // The WorldSettings tries to stay at index 0
    NewActors.Add(WorldSettings);

    // Add non-net actors to the NewActors immediately, cache off the net actors to Append after
    for (AActor* Actor : Actors)
    {
        if (Actor != nullptr && Actor != WorldSettings && !Actor->IsPendingKill())
        {
            if (IsNetActor(Actor))
            {
                NewNetActors.Add(Actor);
            }
            else
            {
                NewActors.Add(Actor);
            }
        }
    }

    iFirstNetRelevantActor = NewActors.Num();
    NewActors.Append(MoveTemp(NewNetActors));
    Actors = MoveTemp(NewActors);

    if (OwningWorld != nullptr)
    {
        if (!OwningWorld->IsGameWorld())
        {
            iFirstNetRelevantActor = 0;
        }

        for (int32 i = iFirstNetRelevantActor; i < Actors.Num(); i++)
        {
            if (Actors[i] != nullptr)
            {
                OwningWorld->AddNetworkActor(Actors[i]);
            }
        }
    }
  }
  ```
  </details>
  
  5. World: The World in UE5 is like the Earth—it’s the container that holds all levels. A World must include a `Persistent Level`, which functions like the central authority, similar to the United Nations overseeing multiple countries (levels).
  ![WorldAndLevel](https://github.com/user-attachments/assets/d94bbf72-702b-4f22-a403-d6811b70be33)

  6. WorldContext: Identifies the Active World: Facilitates Access to World-Specific Systems(gamemode, gamestate, level, actors); Supports Multi-World Scenarios(Persistence level, sublevel); Enables Blueprint Functionality(Get Actor of Class, Spawn Actor from Class, or Get Game Mode).\
  ![WorldContextAndWorld](https://github.com/user-attachments/assets/0f33b91f-aae0-408c-8acf-04157e644a87)
  8. GameInstance: holds the current WorldContext and other information about the entire game.\
    ![GameInstance](https://github.com/user-attachments/assets/318ab8e3-6d18-4d78-84c5-e50625057a09)
  9. Engine: `UGameEngine` and `UEditorEngine`. UE's editor is a game! We are creating another game of our own inside the game that is the editor!
     In different modes, UE selects a specific Engine class based on the build environment, and the base class UEngine stores all Worlds through a WorldList; in `Standalone Game mode`, UGameEngine creates the only GameWorld and directly stores the GameInstance pointer, whereas in the editor, the `EditorWorld` is used only for preview and does not have an OwningGameInstance — only the PlayWorld indirectly holds the GameInstance. 
  10. GamePlayStatics: UE provides a series of static functions in Blueprints through the UGameplayStatics class (such as GetPlayerController, SpawnActor, OpenLevel, etc.), making it convenient for developers to control levels and the world without directly dealing with low-level C++. It serves as an important entry point for accessing engine functionalities in Blueprints. 
  11. Summary: UEngine\
     └── UGameInstance\
       └── UWorld\
          └── ULevel\
            └── AActor\
                └── UComponent
      
## 14, Lamda Function.
  Basic Logic:
  ```c++
  [capture-clause](parameters) -> return_type { function_body }
  ```
  Example:
  ```c++
  int x = 10;
  auto incrementX = [&x]() { x++; };
  incrementX();  // x is now 11
  ```
## 15, Gameplay Ability System.
  A Sample of Creating the Ability Task Node in C++.\
  .h file,
  <details>
  <summary> View Code</summary>
	  
  ```c++
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMyCustomTaskCompletedDelegate);

	UCLASS()
	class YOURGAME_API UAbilityTask_MyCustomTask : public UAbilityTask
	{
    	GENERATED_BODY()

	public:
    	// Constructor
    	UAbilityTask_MyCustomTask(const FObjectInitializer& ObjectInitializer);

    	// Blueprint accessible function to create the task
   	 UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
   	 static UAbilityTask_MyCustomTask* CreateMyCustomTask(UGameplayAbility* OwningAbility, FName TaskInstanceName, float Duration);

    	// Called to activate the task
    	virtual void Activate() override;

    	// Called when the task is ended or canceled
    	virtual void OnDestroy(bool bInOwnerFinished) override;

    	// Delegate that will be called when the task completes
    	UPROPERTY(BlueprintAssignable)
   	 FMyCustomTaskCompletedDelegate OnCompleted;

	protected:
    	// Parameters for our task
    	float TaskDuration;
    
    	// Internal timer handle
    	FTimerHandle TimerHandle;
    
   	 // Timer callback
    	void OnTimerCompleted();
	};  
  ```
  </details>

  .cpp file,
  <details>
  <summary> View Code</summary>
	  
  ```c++
	UAbilityTask_MyCustomTask::UAbilityTask_MyCustomTask(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer), TaskDuration(1.0f)
	{
    		// Initialize any properties here
	}

	UAbilityTask_MyCustomTask* UAbilityTask_MyCustomTask::CreateMyCustomTask(UGameplayAbility* OwningAbility, FName TaskInstanceName, float Duration)
	{
   	 UAbilityTask_MyCustomTask* MyObj = NewAbilityTask<UAbilityTask_MyCustomTask>(OwningAbility, TaskInstanceName);
   	 MyObj->TaskDuration = Duration;
	    return MyObj;
	}

	void UAbilityTask_MyCustomTask::Activate()
	{
   	 // Call Super::Activate() first as it validates the ability is still active
   	 Super::Activate();

    	// Make sure we have a valid world
   	 if (GetWorld())
    	{
       	 // Set up a timer to complete the task after the specified duration
       	 GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTask_MyCustomTask::OnTimerCompleted, TaskDuration, false);
    	}
   	 else
   	 {
      	  // If we don't have a valid world, end the task immediately
       	 EndTask();
   	 }
	}

	void UAbilityTask_MyCustomTask::OnDestroy(bool bInOwnerFinished)
	{
   	 // Clean up any timers or resources
  	  if (GetWorld())
   	 {
   	     GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
  	  }
	
   	 Super::OnDestroy(bInOwnerFinished);
	}

	void UAbilityTask_MyCustomTask::OnTimerCompleted()
	{
   	 // Broadcast our completion delegate
   	 if (ShouldBroadcastAbilityTaskDelegates())
   	 {
   	     OnCompleted.Broadcast();
   	 }
    
   	 // End the task
   	 EndTask();
	}
  ```
  </details>

  Go to chapter 5.24, Execute Ability Task On Tick, it has an example.

## 16, Pawn
   1. Component: You can think of an Actor as being built from components, and components can be thought of as similar to functions.
   2. Actor: If UE is a big country, then Actor is undoubtedly the biggest nation in terms of population.
   3. Pawn: An actor that our player interacts with. It contains 3 blocks of basic template method interfaces: 1, controlled by `controller`; 2, `PhysicsCollision` display; 3, `MovementInput`'s basic response interface.\
      ![Pawn](https://github.com/user-attachments/assets/8104740a-6b18-49c2-ab51-28f5b2a88a15)
   4. DefaultPawn， SpectatorPawn， Character: Here's a concise English summary of your content in a paragraph:
	Unreal Engine provides several types of Pawns to simplify character and camera setup. **DefaultPawn** is a convenient starting point that comes pre-equipped with a movement component, a spherical collision component, and a static mesh—essentially a ready-made package to avoid building everything from scratch. **SpectatorPawn**, derived from DefaultPawn, is designed for non-interactive players like spectators. It removes the mesh display, disables collisions, and uses a movement component suited for free-floating camera control without gravity. **Character** is a more specialized version of Pawn, tailored for humanoid gameplay. It includes a CharacterMovementComponent for realistic walking, a CapsuleComponent for accurate collision, and a 	SkeletalMesh for animations. While beginners may be unsure whether to use Pawn or Character, the rule of thumb is: use **Character** for humanoid characters with skeletons, and **Pawn** for more abstract or non-humanoid setups like VR hands. Character is essentially an enhanced Pawn, while Pawn offers greater flexibility for custom behaviors.\
	![DefaultPawnAndCharacter](https://github.com/user-attachments/assets/d78c666a-a70d-449d-88de-106870f4a40c)

## 17,

 
  
  
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
  - Create a spring arm and a camera
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
    WHY: In a small project, we can use traditional action binding in .h and a callback. However, if in a big project, it will be complicated to handle a bunch of declared actions.
  - Edit -> Project Settings -> GameplayTags
    ![Screenshot 2025-01-12 195446](https://github.com/user-attachments/assets/cb0f33fd-6ad3-4500-8227-7b127c0f6c9f)
  - create an empty C++ named WarriorGameplayTags\
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
Then, the rest is easy, we need to find `LocalPlayer` by `GetController<> -> GetLocalPlayer()`
```c++
ULocalPlayer* LocalPlayer = GetController<APlayerController>() -> GetLocalPlayer();
UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);                                                                                                       

check(Subsystem);

Subsystem -> AddMappingContext(InputConfigDataAsset -> DefaultMappingContext, 0);
```
Step 2, Cast `WarriorInputComponent` to `PlayerInputComponent` by using `CastChecked<>`
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
  Step 1, enable the gameplay system plugin\
  ![Screenshot 2025-01-23 093439](https://github.com/user-attachments/assets/87399900-6750-4345-87a6-d59fe74aeed5)\
  In Warrior.Build.cs file, add "GameplayTasks" inside the code `PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject",...`
  Step2, create AbilitySystem in c++\
  Create a new public name `WarriorGameplayAbilityComponent` in the new folder `AbilitySystem`\
  Step3, create a new c++ AttibuteSet\
  Create a new public name, `WarriorAttributeSet`, in the new folder `AbilitySystem`\
  Step 4, Add these new c++ classes into `WarriorBaseCharacter.h`\
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
  Using `InitAbilityActorInfo()` to set the logical "owner" of the AbilitySystemComponent and he "physical" actor that performs actions\
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
  Step 8, Detect whether the GAS works in the main character\
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
  From step 10, we build the abilitysystemcomponent, In here, we need to give the ability to the character, which requires new stuff to attach into abilitysystemcomponent\
  ![Screenshot_20250124_102133_Samsung capture](https://github.com/user-attachments/assets/5e1f738a-5e0c-40b0-812f-bbdb7fd91251)
  Step 1, create a new c++ `gameplayability` name `warriorgameplayability` in a new folder `abilities`\
  Step 2, compiler the GameplayAbility policy
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
  Step 5, Create a new blueprint\
  Go to ue's content, create a Shared/GameplayAbility folder, then select gameplay/gameplayability blueprint, rename GA_Shared_SpawnWeapon.\
  Change the Warrior Ability|Ability Activation Policy to `On Given`.
- 12, Create weapon Class\
  In the process of setting up the weapon class, we need to understand the weapon class structure:\
  `WarriorWeaponBase`(Handles damage detection) -> `WarriorHeroWeapon`(Weapon data unique to player)\
  In C++, create a new class called `WarriorWeaponBase` under the folder items/weapons\
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
  In shared/GA_Shared_GameplayAbility, right-click in blueprint, `Spawn Actor from class`, \
  create a new variable, named `WeaponClassToSpawn`, then changes its type to `Warrior Weapon Base`, and its `Change to:` should be `Class Reference`\
  Spilt the `Spawn Transformation`\
  Collision Handling should be `Try to adjust, but always spawn`\
  Attempt the `Spawn Actor from class`. Create `Get Avatar from actor info`, connect it to `Owner`, and use `cast to pawn`(convert to pure), to connect to investigator\
  Connect `Event ActivateAbility` to `SpawnActor`\
  create `Is Valid`, drag `return value`, create `Attach Actor To Component`\
  Connect `GetSkeletalMeshComponentFromActorInfo` to `Parent`\
  Promote a new variable for `Socket`, name `Socket Name Attach To`\
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
# 3, Hero Combat
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
  10. Create the attack type tag, in WarriorGameplayTags.h
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
  1. Find the material of the enemy\
  ![Screenshot 2025-03-09 115851](https://github.com/user-attachments/assets/a92b7633-eda4-406f-869e-75cbd42ce14f)\
  2. Create material instance\
  ![Screenshot 2025-03-09 120002](https://github.com/user-attachments/assets/f2be06d1-f8ba-4b41-9111-499a2466ff8d)\
  3. Design the color and add it to the skeleton\
  ![Screenshot 2025-03-09 122123](https://github.com/user-attachments/assets/7cba818a-7ec0-44f9-a9e1-fb48144a0236)\
  4. Add the material color change in GA_HitReact\
  ![Screenshot 2025-03-09 122734](https://github.com/user-attachments/assets/b2762753-e3c2-4a52-847d-9d7df2bd3a8b)\
  ![Screenshot 2025-03-09 122724](https://github.com/user-attachments/assets/371797e1-1173-4f01-b609-5b8a82549c1a)
- 47, Hit Fix - Hit Pause\
  Purpose: Add a hit pause to enhance the hit reaction feeling.\
  1. In WarriorGameplayTags.h
  ```c++
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_HitPause);
  ...
  WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_HitPause);
  ```
  2. Fill the .cpp\
  3. Create a new gameplay ability blueprint named `GA_Hero_Hitpasuse`\
  4. Change the class settings.\
  ![Screenshot 2025-03-09 180418](https://github.com/user-attachments/assets/f04e1972-793a-4426-8364-b81843394716)\
  ![Screenshot 2025-03-09 180423](https://github.com/user-attachments/assets/e2b3679a-4e31-4f95-bda1-13faae0cd2c6)\
  ![Screenshot 2025-03-09 180428](https://github.com/user-attachments/assets/bb17c3d7-4aee-42fa-9a57-726dd7f03a06)\
  5. ![Screenshot 2025-03-09 180653](https://github.com/user-attachments/assets/9c022533-4f1e-4e40-b9c4-045d040c0501)\
  6. Add it to the DA_Hero\
  ![Screenshot 2025-03-09 180748](https://github.com/user-attachments/assets/bc5cc43a-a5b1-45a2-aa6f-498f8ede2102)\
  7. In HeroCombatComponent.cpp
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
  Purpose: add camera shake by using blueprint\
  1. Create a new camerashake blueprint named `Camerashake_HeroMelle`
  2, In GA_Hero_Hitpasuse, add this camerashake blueprint\
  ![Screenshot 2025-03-09 182844](https://github.com/user-attachments/assets/91a87182-1a4e-4e17-aeba-a3f10e80e9b5)\
  3. Customize your camera shake.
- 49, Sound FX - Hit React Sound\
  Purpose: Enable the hit React Sound play and use currency to allow only one sound to play.\
  1. Give a new notify sound effect to the AM_enemy\
  2. Create a new blueprint concurrency named `Concurrency_OneAtATime`\
  ![Screenshot 2025-03-09 194301](https://github.com/user-attachments/assets/46d572dc-76b9-489d-ba28-0b649f14ef33)\
  3. Find that attached sound, attach the concurrency \
  ![Screenshot 2025-03-09 194514](https://github.com/user-attachments/assets/83b4ae54-560d-4c23-a525-67c097ab17cc)\
- 50, Sound FX - Melee Hit Sound\
  Purpose: In this situation, we can not use addNotify to play this sound, because it only happens when we hit something.\
  So, we need to use `GameplayCues` to play that sound when this action happens.\
  1. Create a new folder named `GameplayCues` under the folder Content\
  2. Create a new blueprint based on GameplycueNotify_Static named `GC_Hero_AxeHit`\
  3. In GameplayCueTag, create a new gameplayTag.\
  ![Screenshot 2025-03-09 200709](https://github.com/user-attachments/assets/c210d7a8-2e26-4cbe-b2fb-adaa1f681c35)\
  4. In GC_Hero_AxeHit, create a new execution blueprint.\
  ![Screenshot 2025-03-09 201108](https://github.com/user-attachments/assets/06845515-57a0-434e-b114-f214fce9009e)\
  5, In GA_Hero_LightAttackMaster,\
  ![Screenshot 2025-03-09 201258](https://github.com/user-attachments/assets/78f3f37a-1edf-4a4e-8cb4-0b4f92934e7a)\
  6, In Config/DefaultGame.ini
  ```c++
  GameplayCueNotifyPaths = "/Game/GameplayCues"
  ```
  7, Add the execute gamplaycue on Owner, let the WeaponSoundGameplayCueTag be null(same as light attack)\
  ![Screenshot 2025-03-09 202228](https://github.com/user-attachments/assets/3c7d5689-6784-4afe-8198-cfafd2ab5a1a)\
  8, Then change all child blueprints of heavy/light attack, make sure their WeaponSoundGameplayCueTag is gameplaycue.meleehit.axehit
- 51, Enemy Death Ability\
  Purpose: Add a tag to the actor and use it to trigger the play death montage.\
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
  4. Create a new blueprint based on Gameplay Blueprint, named GA_Enemy_Death_Base\
  5,![Screenshot 2025-03-10 191009](https://github.com/user-attachments/assets/1a1ef7ba-3793-454a-ac58-a85d44c8ffed)\
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
  11, make sure all animations' enable motion is checked.\
  ![Screenshot 2025-03-10 192543](https://github.com/user-attachments/assets/11bdfe32-4ced-49b5-9fc4-9eb564539a39)\
  ![Screenshot 2025-03-10 192638](https://github.com/user-attachments/assets/c311e87e-8622-4bce-ae53-ecabaa9bcddb)\
  12. After creating the animation montage, add them all\
  ![Screenshot 2025-03-10 192847](https://github.com/user-attachments/assets/42c20146-1b91-4a43-b65f-b0c808e5cd88)\
- 52, BP Death Interface\
  Purpose: Instead of using cast to, use an interface to enable some specific function.\
  1,

- 53, Dissolve Material FX\
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
- 63, Enemy Health Widget Component\
  Purpose: using c++ attach the widget component slot in bp_guadrain.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/2d1433764b0bb15d5cf6ab9adf67db55690f0dd2)\
  1, attach the new enemy ui widget to the component slot, and select the space to screen. (In User Interface)\
  2, ![Screenshot 2025-03-19 111520](https://github.com/user-attachments/assets/a47ac062-366a-4b00-b2b1-234a6eb634ce)\
- 64, Hide Enemy Health Bar\
  ![Screenshot 2025-03-20 094331](https://github.com/user-attachments/assets/ad812c7d-b44d-4788-9c1d-85020485dfff)\
- 65, Update Weapon Icon\
  Purpose: Enable the show-up/no-show-up weapon icon when the weapon is equipped/unequipped.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/0bcfd1f5353bdddedc8fe8e0db1ed67da9c90078)\
  ![Screenshot 2025-03-24 114103](https://github.com/user-attachments/assets/3cc82a3d-4cb9-4cef-ba76-5fb893019b6d)\
  ![Screenshot 2025-03-24 114219](https://github.com/user-attachments/assets/98cc59a1-cbcd-467a-a0e6-44bf39434249)\
  ![Screenshot 2025-03-24 114228](https://github.com/user-attachments/assets/f90c5631-e746-4c16-93de-cfe3558334fa)\
- 66, Twaeking\
  1, When the icon is loading, it will pop up a blank picture because this image is loading.\
  ![Screenshot 2025-03-24 135736](https://github.com/user-attachments/assets/bebcadcc-be43-46f6-84d3-1369539c9e0f)\
  ![Screenshot 2025-03-24 135743](https://github.com/user-attachments/assets/bc269219-cd2d-40ef-ad2c-f48409a57f61)\
  2. Change the health bar status style.\
# 4, Enemy AI
- Summary: In this part, we do two things: `Enemy AI Controller` and `Behavior Tree`\
  `Enemy AI Controller`: AI Avoidance, perception, Generic Team ID;\
  `Behavior Tree`: BTTask/Service/Decorator, EQS, Straifing, Attack;
- 1, preparing enemy for the combat\
  Purpose: For the avodiance work accurately, we need the capsule component should fit the enemy size.\
  resize the capsule to 1.0, multiply the Capsule Half Height, Capsule Radius by 1.5, and multiply the scale of bp_Guntling_Guadrain to 1.5\
- 2, Crowd Following Component\
  ⚠️ For AI Avoidance: We have two methods to implement it: `AVO Avoidance` and `Detour Crowd Avoidance`\
  RVO (Reciprocal Velocity Obstacles) Avoidance\
  ✅ Best for small groups (5–10 AI).\
  ✅ Works without a NavMesh, adjusts velocity dynamically.\
  ❌ Struggles with large crowds, can get stuck in congestion.\
  Detour Crowd Avoidance\
  ✅ Best for large crowds (10+ AI).\
  ✅ Uses NavMesh, adjusts paths smoothly.\
  ❌ Higher CPU cost, slower reaction to sudden obstacles.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/828d4b72d07d485daa17504cbae49d1067221c83)\
  1, Create a aicontroller c++ and create a blueprint.\
  2, create a child AIC_enemy and attach it to the BP_Gruntling_Guadrain.\
- 3, AI Perception Component\
  Purpose: Create Perception Component.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/e38be6f63c9b5e2afd2d508bfeef6ad88da5928b)\
  For this Perception component, we require the AI sign as the domain sense because we need TwoParams to update the sense detect status and use UFunction to state the required `AActor* Actor, FAIStimulus Stimulus` variables.\
  ![Screenshot 2025-04-01 114018](https://github.com/user-attachments/assets/4fb7f7cb-13ee-4280-aa50-f2b63147b847)\
- 4, Generic Team ID\
  Purpose: Generic Team ID for identification in perception.\
  ⚠️ Hint: WarriorAIcontroller is under AAIController, which is an Inheritance of `IGenericTeamAgentInterface`;
  However, WarriorHeroController is Direct Implementation, which needs a header file, `#include "GenericTeamAgentInterface.h"`.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/214b7a167a2fe5546244697e012d976763cb1474)\
  [video](https://github.com/user-attachments/assets/e466c1c6-6048-48b1-b5ef-2552a51db696)\
- 5, Behavior Tree\
  Purpose: Use the blackboard and behavior tree to enable enemy movement.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/147177d2c9a53a2fa85e7010b639a558deefe2c0)\
  1, Create a new variable in Blackboard and a new behavior tree.\
  ![Screenshot 2025-04-03 195525](https://github.com/user-attachments/assets/ecc4a5e0-539b-4ed9-8bc9-ffc91db98138)\
  2, In AIC_Enemy, enable its blueprint for activating the behavior tree.\
  ![Screenshot 2025-04-03 195835](https://github.com/user-attachments/assets/cf651880-6dfa-404d-a7dd-fe8719a986f0)\
  3, Add a new nav mesh.\
- 6, Configure AI Advoience\
  Purpose: Customize your crowd Advoience, using terminal `AI.Crowd.DebugSelectActors 1` and F8 to check\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/ba94557dc4c89cc9313316a9b7f54de98c04af7f)\
  ![Screenshot 2025-04-06 101116](https://github.com/user-attachments/assets/73fc9f3a-b8da-4603-808b-617ec3ebd80f)\
  ![Screenshot 2025-04-06 101314](https://github.com/user-attachments/assets/6e080450-1fb8-4a31-8ac0-86c2eb7cfb54)\
- ⚠️7, Behavior Tree Nodes Type\
  Purpose: Basic crowd avodiance and easy behavior tree `move to` cannot solve multiple enemies attacking the main character simultaneously.\
  1, Composite Node: Engage Target\
  2, Decorate Node: Determination\
  3, Service Node: Servicing\
  4, Task Node: Tasking\
  ![Screenshot 2025-04-06 105056](https://github.com/user-attachments/assets/37ae5cd4-59b0-41af-a008-8b92159a4e94)\
  ![Screenshot 2025-04-06 105110](https://github.com/user-attachments/assets/40760c70-a1b2-46e4-bf14-4fd7ad128644)\
- 8, Observer Aborts\
  Purpose: Understand `Notify Observer` and `Observer Abort`\
 🔔 Notify Observer\
    On Value Change: Triggers when the blackboard key changes.\
    On Result Change: Triggers when the condition's result (true/false) changes.\
 🚫 Observer Abort\
    None: No abort.\
    Self: Abort this branch.\
    Lower Priority: Abort lower branches.\
    Both: Abort self and lower branches.\
 ![Screenshot 2025-04-06 175910](https://github.com/user-attachments/assets/0d7a4ba1-1bc3-43ff-8e4d-8b553faac755)\
- ⚠️9, Customize your BTService\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/0a3bad3c73e6f83ee1037a3a62667a77638e589d)\
- 10,  Custom EQS\
  Purpose: Enable the guardian to walk around the hostile.\
  1. Create a new `EQS_FindStarfingLocation` and `EQS_TestPawn`, enabling the attachment of the EQS to testpawn.\
  2, create a new `EQSContext_TargetActor` to enable the circle center of `Points: circle`\
  ![Screenshot 2025-04-09 115042](https://github.com/user-attachments/assets/b3a6a0bb-bd43-41a1-99b0-841194e8ce98)\
  ![Screenshot 2025-04-09 115122](https://github.com/user-attachments/assets/bb8304d1-03e5-4e25-94f3-115383392394)\
  ![Screenshot 2025-04-09 115130](https://github.com/user-attachments/assets/398a6ebe-b31f-4ecd-adff-ab6ab1e0d32d)\
  3. Debug the vision issue.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/b9e0ce54e183890cfd744a9f396a56c7a7a757bb)\
- 11, Toggle Strafing State\
  Purpose: Enable the guardian to have different walking speeds when it does strafing.\
  1. In AIC_Enemy_Base, catch the walk speed.\
  ![Screenshot 2025-04-10 085248](https://github.com/user-attachments/assets/170bcfd2-7a9f-43d1-9c63-a160366e19d3)\
  2. Create a new BTTask_ToggleStrafingState, and detect if the walk speed should change.\
  ![Screenshot 2025-04-10 085248](https://github.com/user-attachments/assets/ab1705e1-2f2a-4a6a-8508-4ca24294251e)\
  3. Add the new BTTask service into BT_Guadrain/\
- 12, Calculate Direction\
  Purpose: Using Kismet to catch up the guardian's moving direction and using a helper function that has a gameplay tag as a bool.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/8c94b203880ddbbb0f23d7f2682a5430967f3f96)\
  1. Enable the switch's different animations.\
  ![Screenshot 2025-04-10 092356](https://github.com/user-attachments/assets/6fc58176-c447-4e37-acfa-267ce4fbc9f3)\
- 13, Starfing Blend Space\
  Purpose: Enable based on different locomotion direction and walking speed, to change its blend space.\
  ![Screenshot 2025-04-10 094536](https://github.com/user-attachments/assets/996c2c7b-e541-4709-977c-2d4dd3808a76)\
- 14, Compute Success Chance\
  Purpose: create a new decorator \
  ![Screenshot 2025-04-14 090935](https://github.com/user-attachments/assets/7904afd6-daaa-49c6-8318-1189d96f4c81)\
  ![Screenshot 2025-04-14 090940](https://github.com/user-attachments/assets/a6844dd4-f840-498f-bc5c-808fcb0dd667)\
- 15, Dot Product Test\
  Purpose: Use the dot product to decide where the enemy does strafing location movement\
  ![Screenshot 2025-04-15 085837](https://github.com/user-attachments/assets/e1f9f578-213e-4d81-b87e-c46012da2e88)\
- 16, Enemy Melee Ability\
  Purpose: Enable two new gameplay features for melee attacks.\
  1. Create a gameplayability melee base, and create two new children.\
  ![Screenshot 2025-04-15 090827](https://github.com/user-attachments/assets/545328c3-491c-4c52-92ee-2b7f7d5e11b6)\
  2. Create two new montages based on melee animation and attach them to the gameplayability.\
  3. Attach this new gameplayability to the DA_Guadrain.\
   ![Screenshot 2025-04-15 091012](https://github.com/user-attachments/assets/4f025fe0-de05-4766-9a6c-83aba46145eb)\
- 17, Activate ability by tag\
  Purpose: Create a new dummy enemy to test tag activation by tag. In this case, we need a new bttask_activatebytag.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/cbf2ee238986af016e3baefe88505fd0a7009384)\
  1. Create a new BTTask_ActivateByTag.\
  2, Create a child bp of BP_Guadrain, and copy the AIController, BehaviorTree.\
  3. Reset the AI behavior setting.\
  ![Screenshot 2025-04-16 110532](https://github.com/user-attachments/assets/5b8bddc9-0e87-4f39-84d0-b766685ee48d)\
- 18, Is Target Hostile\
  Purpose: In WarriorWeaponBase, we detect the hitbox-detect basic relationship between attacker and target, which means enemies can attack each other.
  To solve that, we need team members to avoid it from happening.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/1d8317a890642005b1f9e80c628206fc0c97f894)\
  1. Add ANS_Notif in melee attack montages.\
- 19, Notify the Melee Attack\
  Purpose: using `SendGameplayEventToActor` to trigger the ability on the actor.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/1f51771efaa66e278ad9697670547a04d42a89df)\
  1. In GA_EnemyMeleeAttack, enable the play string.\
  ![Screenshot 2025-04-16 165653](https://github.com/user-attachments/assets/6e5ebc71-b837-4197-87d6-4634cb411be9)\
- ⚠️20, MakeEnemyDamageEffectSpecHandle\
  Purpose: Enable the Enemy to Get Damage info. It is similar to the hostile, but it needs `FScalableFloat& InDamageScalableFloat` to show different damage in dynamic level changes.
  Remember that the hero damage is constant; it gets changed by combo.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/215a1f69bbb7ce4a69ca02d55c65a9a3d6ab59a8)\
  1. Create a new function `Handle Apply Damage` in GA_EnemyMeleeAttack_Base.\
  2. Add a new input called `InPlayLoad` in the `Handle Apply Damage`\
  ![Screenshot 2025-04-16 174217](https://github.com/user-attachments/assets/6f71459f-2232-463f-9b60-9fcf493de740)\
- 21, Apply Empty Damage\
  Purpose: Add damage data to the game effect curve table.\
  1. Go to the GT_GuadrainStatus, add `Guardian.LightAttackDamage`, `Guardian.HeavyAttackDamage`.\
  2, attach them in `GA_Enemy_MeleeAttack_1` and `GA_Enemy_MeleeAttack_2`\
- 22, ⚠️Motion Warping\
  Purpose: Using Motion Warping: Dynamically adjust a character's root motion to align to targets.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/39d45258b1ab4125150b9736567ae7664ab12c2f)\
  1. Add the `Motion Warping` as a notification to the montages.\
  ![Screenshot 2025-04-17 100307](https://github.com/user-attachments/assets/67a7b50e-9097-4d45-a8f3-347531565bfc)\
- 23, Update Motion Warping Target\
  Purpose: Update the blackboard key's actor location and enable `add or update warp target from location`.\
  1. Add a new btservice, called `BTService_UpdateMotionWrapTarget`.\
  ![Screenshot 2025-04-17 104202](https://github.com/user-attachments/assets/4a28216f-1dd1-4f10-8383-e760bae49cbc)\
  2, Add this new btservice to the bt_dummy and change the blackboard key to targetactor.\
- 24, ⚠️Construct Native BTTask\
  Purpose: Similar to the `9, customize the BTService`, we create a new C++ BTTask.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/6261ddbb8aec01749381fec42d6a484244c4aee9)\
- 25, Rotate to Target And Attack\
  Purpose: `ExecutedTask`, `TickTask`, `HasReachedAnglePrecision` to detect the rotation situation and decision.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/0cd50778410fd6de527198b3fa1f590f6cbce28c)\
- 26, Melee Attack Branch\
  ![Screenshot 2025-04-24 102108](https://github.com/user-attachments/assets/9ad6584f-c94b-4c0f-bf32-4c8e4246ff11)\
- 27, Does Actor Have Under Attack Decorater\
  Purpose: Create a new Decorater to detect whether it is under attack.\
  ![Screenshot 2025-04-24 102354](https://github.com/user-attachments/assets/740f36dc-2ac5-47d6-8104-40841b98430b)\
  ![Screenshot 2025-04-24 102736](https://github.com/user-attachments/assets/ff481a82-f4b9-400c-a849-c903e387bed4)\
- 28, Duration GameplayEffect\
  Purpose: Customize the GE_UnderAttack based on the Duration in duration, and apply it in the GA_HitReact\
  ![Screenshot 2025-04-24 103042](https://github.com/user-attachments/assets/80f4c225-4804-42ed-983e-a1fe919f3d2e)\
  ![Screenshot 2025-04-24 103214](https://github.com/user-attachments/assets/19a31532-8f8d-4a8b-9fb3-b05b0fb5c3b0)\
- 29, Should Abort All Logic And Assign the HitSound.\
  Purpose: Add a new decorator to abort all other logic when hp is lower than 0.\
  ![Screenshot 2025-04-24 103649](https://github.com/user-attachments/assets/28e6bf7c-02e4-4545-a227-a844d62d9993)\
  1. Attach it to the new sequence.\
  2. Customize the Guardian sound FX based on your preferred sound effect. same as `49, Sound FX - Hit React Sound`\
# 5, Hero Combat Ability
- Summary: In this part, we do 5 things: Directional Rolling,  Directional Hit React, Block, Target Lock, Hero Death\
- 1, Two Key Input Action
  Purpose: Enable the use of two key input actions, for example, hostile can do directional rolling based on two keys pressed [w + space bar]\
  1. Create the gameplay ability tags.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/a9fdd22eba08848cbbe71e34126c64a42a3176bc)
  2. Create the Gameplayability blueprint[based on the warriorHerogameplayability]
  3. Create the new input action, bind it to the IMC [Trigger should be chored Actions]
  ![Screenshot 2025-04-25 102836](https://github.com/user-attachments/assets/c56b75e6-3a37-403c-ab91-728793449095)
  4. Add this new input action to DA_InputConfig
  5. Add the new tag inside the DA_Hero
  6. Grant the Gameplayability blueprint
  ![Screenshot 2025-04-25 102740](https://github.com/user-attachments/assets/893b165b-f688-490d-aa35-d13f171936c0)
- 2, Get Last Movement Input
  Purpose: using the blueprint `Add Or Update Wrap Target Location and Rotation` to update the last input direction based on the new Add MotionNotify state on the montage.\
  1. In GA_Roll, enable the play montage and wait. (Required a new montage AM_Hero_Roll)
  ![Screenshot 2025-05-01 094338](https://github.com/user-attachments/assets/33e49012-1ca8-4509-b699-e10b775d8ca4)
  2. Finish the last blueprint.
  ![Screenshot 2025-05-01 094301](https://github.com/user-attachments/assets/acb42d44-f13a-4240-98bc-5c02ed1602cb)\
  ![Screenshot 2025-05-01 094308](https://github.com/user-attachments/assets/b626bbb3-72c0-4892-99da-810a852b4f14)
- 3, Get Last Movement Distance
  Purpose: In 2, we have updated the Rotation. Here, we need to customize the distance by using `Line Trace By Objects`
  1. Add a new notifystate motionwarping
  ![Screenshot 2025-05-01 094338](https://github.com/user-attachments/assets/3432442c-6fdc-48f3-a8ad-7e1f4bc669cd)
  2. Fill up the `Line Trace By Objects`
  ![Screenshot 2025-05-01 101828](https://github.com/user-attachments/assets/41b2370d-dc8f-4d81-9628-9f7b6666724c)
- 4, Get Value At Level\
  Purpose: Customize the rolling distance based on different hero levels.
  1. Enable the get level function, created in C++.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/0a3009ac93a4acc7a77abddff5f737683c73df37)\
  2. Create a new `Scalable Float` variable, using `Get ability level` to catch the required level and rebuild the blueprint.\
  ![Screenshot 2025-05-02 104330](https://github.com/user-attachments/assets/445e5a56-cc9a-4d25-98fb-19bd168ef955)
- 5, Hero Hit React Ability\
  Purpose: enable the directional hit react\
  1. Build the Hit React Ability Blueprint
  ![Screenshot 2025-05-04 163035](https://github.com/user-attachments/assets/f1d016e9-8202-46a8-8cd6-f7e0b3caa810)\
  2. Create the HitReact Montage, remember to change the slot name to `upbodyslot`.
- 6, Trigger Hero Hit React\
  Purpose: enable this gameplayability to play montage and get gameplayevent from enemy melee attack.\
  1. Fill up the GA_Hero_Hitreact.\
  ![Screenshot 2025-05-04 170357](https://github.com/user-attachments/assets/7619005f-fb47-4bf8-9b33-f05d1bbab765)\
  2. Enable the hero character to use this reactive gameplay ability in DA_Hero.\
  ![Screenshot 2025-05-04 170455](https://github.com/user-attachments/assets/727e0fa6-641c-4508-928d-d4a15bc24a9b)\
  3. Enable the DummyEnemy to attack, fix the behavior tree.\
  4. After the enemy's Melee attack, catch up the gameplay event tag from the enemy.\
  ![Screenshot 2025-05-04 170714](https://github.com/user-attachments/assets/638ea840-927c-4068-b7fc-9d5f099f6abe)
- 7, Hero Hit FX\
  Purpose: enable the Hit react FX\
  1. Add the ComputFX into the material, and get its child\
  ![Screenshot 2025-05-04 183841](https://github.com/user-attachments/assets/f98ae94a-0fa9-4b74-8822-723d24b30701)
  2. Fix the GA_HitReact
  ![Screenshot 2025-05-04 185101](https://github.com/user-attachments/assets/05a77066-9853-4095-8eba-485c10dd586b)\
  ![Screenshot 2025-05-04 185105](https://github.com/user-attachments/assets/51445a1a-c971-46b3-a299-adf8534684b0)
- 8, Compute Hit React Direction\
  Purpose: using the cross product to find the hit direction. {In UE5, it uses the left-hand cross product. To simplify decision-making in code or animation blueprints, this sign is often converted to a standardized value:
  If Z < 0: Set to -1 (indicating left).
  If Z ≥ 0: Set to 1 (indicating right). }
  1. Create a new C++ function to compute the hit direction.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/d0511684802ebb13a4433bfed9735483e6293dd6)\
  2. Pop up the compute hit react direction for debugging.
  ![Screenshot 2025-05-04 192754](https://github.com/user-attachments/assets/4855402f-df1d-4184-b979-60875e3c2470)
- 9, ⚠️ Hit React Tags\
  Purpose: Hit from front (-45, 45); Hit from right (45, 135); Hit from back [ < -135 or > 135]; Hit from left (-135, -45).\
  1. Add four direction tags and return different sample tags based on different hit angles.\
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/f8609f2cb1af9cd72f923b2242e41326c35ab2cf)
- 10, ⚠️ Switch On Gameplay Tags + Fix the hit react animation issue\
  Purpose: Use the blueprint `Switch On Gameplay Tags` to avoid animation issues when getting hit.\
  1. Add the `Switch On Gameplay Tags`.
  ![Screenshot 2025-05-05 182159](https://github.com/user-attachments/assets/f8e1a34f-a2aa-4c08-9161-0626d0fcd561)
  2. Fix the issue: we blend the upper body to locomotion; however, when the hostle is not moving, it should display full-body hit react animation.
  3. Using the `Blend pose by bool` and the anim variable: acceleration to determine whether to display hit react fullbody or blend upperbody and fullbody.
  ![Screenshot 2025-05-05 183349](https://github.com/user-attachments/assets/9b1339c0-57b7-4603-b52e-54d7c9faa67a)
- 11, ⚠️ Hero Block Ability\
  Purpose: New Ability Access, Must Be Held, Loop Animation, Visual Cue, Block Logic\
  1. Create necessary gameplaytags. [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/2ff771d51ed2db883fce2c40c0de8e4912cbe1ca)
  2. Create a new GA_Block.
  3. Create a new IA_Block.
  4. Add new Inputability in DA_InputConfig.
  5. Attach this new Block InputAbility in BP_HeroAxe(Only be activatable when weapon eqquiped)
  6. Link the Input Button in IMC_Weapon.
- 12, ⚠️Enable Block montage + Add GameplayCue
  1. Create a block montage, change its slot name to upper, and pause it for looping.
     ![Screenshot 2025-05-08 091611](https://github.com/user-attachments/assets/a2de4b7a-ad40-428a-8259-58838008b596)
  2. Add block montage in gameplayability_block.
      ![Screenshot 2025-05-08 091739](https://github.com/user-attachments/assets/6c79602c-c0dc-4eb0-bef6-2c1f770abc47)
  3. In GA_HitReact, add cancelled abilities `Player.ability.block`.
     ![Screenshot 2025-05-08 091859](https://github.com/user-attachments/assets/361ea96f-26ec-4575-b592-0b95085990e6)
  4. Create a new GameplayCue_Actor for MagicShield, and manage its class settings.
     ![Screenshot 2025-05-08 093202](https://github.com/user-attachments/assets/f7c50596-b834-4c67-9a16-e2df6bd2b7e8)
  5. In GA_Block, using `Add GameplayCueWithParameter To Actor`.
     ![Screenshot 2025-05-08 093619](https://github.com/user-attachments/assets/e778dd58-d39c-4490-9c9b-10da1ccdb09b)
  6. In GC_MagicShield, compute two functions: while and on remove.{remeber to add the new socket in Skeleton}
     ![Screenshot 2025-05-08 095309](https://github.com/user-attachments/assets/28d346b0-8aa6-484e-af84-3e41ce45ad2f)\
     ![Screenshot 2025-05-08 095623](https://github.com/user-attachments/assets/20939364-16dd-43d9-bdb9-7ff6c21ab67e)
  7. In the while, we use `Spawn System Attached`, `play sound at location` to add sound FX and visual FX. In Remove, we need to destroy the component.
- 13, Is Valid Block
  Purpose: Using the dot product for the attacker.forward vector, and player. forward vector, if near 0, then it is a valid block.
  1. [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/72cbad3fe0b7bfbf59f79ad326b9bc617dd7d114)
- 14, Successful Block
  Purpose: Notify and handle the successful block.
  1. [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/e9399c4325ff3d4275c5761805e5cb5dceec567b)
  2. In GA_Block, use `Set Actor Rotation`, `Apply Roost constant force`, and `Execute Gameplaycuewithparams on owner` to face the attacker, apply force back, and play sound/VFX on location.
  ![Screenshot 2025-05-11 115206](https://github.com/user-attachments/assets/9a82bca6-5917-4103-a1e6-4651e4d2b850)
  3. Create a new GC_Hero_Successful block.
  ![Screenshot 2025-05-11 115216](https://github.com/user-attachments/assets/92018b39-61bb-455c-9f2c-1ed21a607b84)
- 15, Is Perfect Block
  Purpose: using the comparison between the block start time and required check time to decide the perfect block.
  ![Screenshot 2025-05-11 165347](https://github.com/user-attachments/assets/d7ea968b-2d48-4c01-b2fa-9e086355a06f)\
  ![Screenshot 2025-05-11 165340](https://github.com/user-attachments/assets/ece08ddb-fd82-4403-a376-69f4b9f3aeb1)
- 16, Notify Perfect Block
  Purpose: Add a VFX to show a special VFX to notify the player who did a perfect block.
  ![Screenshot 2025-05-11 173648](https://github.com/user-attachments/assets/a99f7566-9e5d-4555-9749-608578bcd0a0)\
  ![Screenshot 2025-05-11 173721](https://github.com/user-attachments/assets/158f0f7d-f492-4dc6-afb8-43846af9cca4)
- 17, Initiate Counter Attack
  Purpose: If a perfect block, then do Jump To finisher Attack.
  1. In GS_Block, a new event is used to detect a short CD for the next finisher attack.
  ![Screenshot 2025-05-11 183912](https://github.com/user-attachments/assets/0f501b58-3391-4e04-b99c-e5103939c109)\
  ![Screenshot 2025-05-11 183917](https://github.com/user-attachments/assets/f2184be9-fbb6-456d-ba0a-3d9f9716ce11)\
  ![Screenshot 2025-05-11 184227](https://github.com/user-attachments/assets/585a2252-18d9-433f-89a3-dd6891ce69c2)
  2. In GS_LightAttackMaster, if perfect block is enabled, then use the combo length to set the combo count.
  ![Screenshot 2025-05-11 183926](https://github.com/user-attachments/assets/d4fc04b2-7bcb-4e94-93a8-239a9afa7560)
- 18, Target Lock 
  Purpose: To enable this function, we need to have toggleable ability, get available targets, draw/update the target lock widget, switch animation pose, and switch target.
  1. Create a new child WarriorGameplayAbility_Lock based on WarriorHeroGameplayAbility.
  2. Add GameplayTags based on the new gameplayability TargetLock.
  3. Create a new GA_Hero_TargetLock based on this new c++.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/74023193889b1b7d3d2db2adc8d42d911b6b911b)\
  ![Screenshot 2025-05-12 094355](https://github.com/user-attachments/assets/4552b043-7d76-4b0d-9cac-7ee6335c8c8c)
- 19, Target Lock
  1. Enable the InputTag_Toggleable.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/c299a8ac7c63919db21473394e3fa08e942ced88)
  2. new IA_TargetLock, bind the new GameplayAbility in weapon_axe, put Input_TargetLock in DA_InputConfig, bind button in IMC_Axe.
- 20, ⚠️ Get Avaliable Targets
  Purpose: Using `BoxTraceMultiForObjects` to detect in-box pawns.
  <details>
  <summary> View Code</summary>

  ```c++
  UFUNCTION(BlueprintCallable, Category = "Collision", meta = (bIgnoreSelf = "true", WorldContext="WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "Multi Box Trace For Objects", AdvancedDisplay="TraceColor,TraceHitColor,DrawTime", Keywords="sweep"))
  static ENGINE_API bool BoxTraceMultiForObjects(const UObject* WorldContextObject, const FVector Start, const FVector End, const FVector HalfSize, const FRotator Orientation, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf, FLinearColor TraceColor = FLinearColor::Red, FLinearColor TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);

  /**
   * Sweeps a capsule along the given line and returns the first hit encountered.
   * This only finds objects that are of a type specified by ObjectTypes.
   * 
   * @param WorldContext	World context
   * @param Start			Start of line segment.
   * @param End			End of line segment.
   * @param Radius		Radius of the capsule to sweep
   * @param HalfHeight	Distance from center of capsule to tip of hemisphere endcap.
   * @param ObjectTypes	Array of Object Types to trace 
   * @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
   * @param OutHit		Properties of the trace hit.
   * @return				True if there was a hit, false otherwise.
   */
  ```
  </details>
  
  1. [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/4b0a1131f485b2f8b9c4ac85ffabcef7ef83ddb5)
  2. ![Screenshot 2025-05-13 092802](https://github.com/user-attachments/assets/2b571fab-0183-41f1-9530-31cd71beb5f7)
- 21, ⚠️ Get Nearest Target
  1. [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/8d29b9962f31fd6483020662420b76a4ccb45eb3)
- 22, ⚠️ Draw Target Lock Widget And Play it
  1. Create a new WDG_TargetLock.
  2. Using template `Createwidget`, `UUserWidget::AddToViewport()`, `UWidget::RemoveFromParent()`
     <details>
     <summary> View Code</summary>
	     
       ```c++
	 template <typename WidgetT = UUserWidget, typename OwnerType = UObject>
  	 WidgetT* CreateWidget(OwnerType OwningObject, TSubclassOf<UUserWidget> UserWidgetClass = WidgetT::StaticClass(), FName WidgetName = NAME_None)     
       ```
     </details>
     
     [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/7ae2bcfe8e76ad589d11ce33ab2dff7edbf9e694)
- 23, ⚠️⚠️Set Target Widget By Position
  Purpose: 1, using `ProjectWorldLocationToWidgetPosition` to convert 3d to 2d; 2, using `DrawnTargetLockWidget->WidgetTree->ForEachWidget(Lamda function)` to capture the sizebox's size.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/bd5df4199b36b996ffd810a697b7050b32bbd461)
  <details>
     <summary> View Code</summary>
	     
       ```c++
	  bool UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(APlayerController* PlayerController, FVector WorldLocation, FVector2D& ViewportPosition, bool bPlayerViewportRelative)
      {
	  FVector ScreenPosition3D;
	  const bool bSuccess = ProjectWorldLocationToWidgetPositionWithDistance(PlayerController, WorldLocation, ScreenPosition3D, bPlayerViewportRelative);
	  ViewportPosition = FVector2D(ScreenPosition3D.X, ScreenPosition3D.Y);
	  return bSuccess;
      }

      bool UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPositionWithDistance(APlayerController* PlayerController, FVector WorldLocation, FVector& ViewportPosition, bool bPlayerViewportRelative)
      {	
	  if ( PlayerController )
	  {
		FVector PixelLocation;
		const bool bProjected = PlayerController->ProjectWorldLocationToScreenWithDistance(WorldLocation, PixelLocation, bPlayerViewportRelative);

		if ( bProjected )
		{
			// Round the pixel projected value to reduce jittering due to layout rounding,
			// I do this before I remove scaling, because scaling is going to be applied later
			// in the opposite direction, so as long as we round, before inverse scale, scale should
			// result in more or less the same value, especially after slate does layout rounding.
			FVector2D ScreenPosition(FMath::RoundToInt(PixelLocation.X), FMath::RoundToInt(PixelLocation.Y));

			FVector2D ViewportPosition2D;
			USlateBlueprintLibrary::ScreenToViewport(PlayerController, ScreenPosition, ViewportPosition2D);
			ViewportPosition.X = ViewportPosition2D.X;
			ViewportPosition.Y = ViewportPosition2D.Y;
			ViewportPosition.Z = PixelLocation.Z;

			return true;
		}
	  }   
       ```
     </details>
     
- 24, ⚠️⚠️⚠️Execute Ability Task On Tick
  Purpose: In GA_TargetLock, there is no blueprint node for the tick function. We need to create a new ability task node that includes ticks.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/f0d28376ab6bf9617415c0cf8a4914a60d350cef)

- 25, ⚠️⚠️⚠️ On Target Lock Tick
  Purpose: In 24, we have enabled a new ability node for getting each tick per frame. And here we need to update each frame, the target lock is targeting the closest one.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/2f387b1c5f7419607d0549e88015a36c032b85c1)
  ![Screenshot 2025-05-14 163252](https://github.com/user-attachments/assets/615d33c2-6b0a-4d75-bc50-6c762a0a36cd)

- 26, ⚠️⚠️ Orient to Target with target lock
  Purpose: 1, In cleanup, initialize the `DrawnTargetLockWidget` and `TargetLockWidgetS`; 2, When doing the orienting, stop auto orienting if the hero is doing some specific actions, and force the player controller,
  	      actor rotation to face the enemy using `FMath::RInterpTo( const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed)`
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/c0dd15336507156ed8e82ba867e99d27f9666afb)
  
- 27, Switch To Target Pose
  Purpose: Create a new blend space for Directional Locomotion TargetLock. (If I forget how to do animation creation, go back to chapter 1.)
  1. ![Screenshot 2025-05-14 200027](https://github.com/user-attachments/assets/057ed410-5bbc-400c-8bf9-b708706e7a55)
  2. using `blend pose by bool`, `does owner have tag`, `blend space by player`, expose a pin(help to get the blend space as a variable ), `property access` (to get  required blend space properties)\
     ![Screenshot 2025-05-14 200645](https://github.com/user-attachments/assets/104c0a0d-d5c7-4442-a8cc-51ef3cc4bd92)

- 28, Target Lock Speed
  Purpose: Character has two walking speeds: locking target walk speed, normal walking speed. However, when he is doing a locking target walk, its speed will be higher than the max target lock walking speed, making the character do normal walking. We need to use C++ to limit the maximum lock target walk speed to solve it.
  [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/00ef2ecba253143510f1ef542910c9205f128d92)
  1. When the character moves left, right, left, right. It does meaningless shaking, to solve this, we have to click the `wrap input`.
     ![Screenshot 2025-05-14 204040](https://github.com/user-attachments/assets/d3f9babf-9ed6-490a-8b56-7a3e01b71626)

- 29, ⚠️⚠️⚠️ Switch Target Input Action
  Purpose: We need to use mouse input directly to control the locking switch, which will quit the control of the character camera.\
  1, Native Input Action; 2, Input Callbacks; 3, New Mapping Context; 4, Notify switch target; 5, Handle switch target
  1. Create the tags and input the callback in warriorHerocharacter. (This is a default input action.)
     [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/6262dff1925c5b95360be6e0408644b56562b859)
  2. Add it to DA_InputConfig(Native Input Action), and create a new Input Action. Make sure its value type is `Axis 2D(Vector2D)`.
 
- 30, New Mapping Context
  Purpose: Using `UEnhancedInputLocalPlayerSubsystem* Subsystem` enables attaching a new IMC to the local player. (Create a new IMC slot in GA_TargetLock)
  1. [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/d501fd3484990830dc39c0f08d4f560f8222e3a7)
  2. Create a new IMC_SwitchTarget and attach it in GA_TargetLock's slot.

- 31, ⚠️⚠️Notify Switch Target
  Purpose: Capture mouse input -> notify Target Lock Ability -> send gameplay event -> play target lock ability -> handle switching
  1. Create a new target event and send GameplayEventToActor.[This event is bound to the button pressed!!!]
     [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/c7b68df5fd82b43b4497cab925df899cd9d87a91)
  2. Do test.\
     ![Screenshot 2025-05-16 105347](https://github.com/user-attachments/assets/aae6c9de-8737-46ba-9c96-d5473243784f)

- 32, ⚠️⚠️⚠️⚠️Handle Switch Target
  Purpose: First, use the cross product.z to detect whether the enemy is on the left or the right; Second, after sorting those actors, place them into the array; Third, use the gameplay event to detect which is which; Fourth, reset the mouse input movement speed.
  [code1 view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/965a8e1f2f6da46a4c7974158a96637030d982d7), [code2 view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/4456ab5f16af3df9648d50542515ae9aedd3ac12)
  1. ![Screenshot 2025-05-16 113330](https://github.com/user-attachments/assets/4bdbee0e-1911-40e1-b26e-a7707a35324d)
  2. ![Screenshot 2025-05-16 113022](https://github.com/user-attachments/assets/e2241d07-8420-4b4d-804d-1ca5bc36115e)
 
- 33, Hero Death + Issue Fixed
  1. Create a new GA_Hero_Death.\
     ![Screenshot 2025-05-19 085603](https://github.com/user-attachments/assets/c582368f-f558-4647-86c2-a203740fe801)
  2. Create two Anime Montages for hero deaths.
  3. [code view](https://github.com/Lazy22zz/UE5-RPG-Combat-Game/commit/993dfa6aeab7f6e58b1f79dc19c51b244a43f0cf)
 
# 6, Ranged Enemy
  Purpose: Starting Weapon, Starting Stats, Combat Abilities, Projectile Ability, Behavior Tree.
- 1, Create a new enemy Glacer
  1. Duplicate new BP_Glacer based on the BP_GuntlingBase, DA_Guadrain, Bp_GuadrainWeapon.
  2. Fix them and attach the property name.

- 2, Glacer Starting Stats
  1. Duplicate the CT_Guadrain, GE_Guadrain, reset those data.
 
- 3, Glacer Hit React
  0. Check back the Chapter 3 Hero Combat, Hit FX for FX setting. 
  1. Fix the ComputDisolveFX in MI_Glacer. (Need to duplicate from Default FX)
  2. Change the dissolveFX in MI_GlacerWeapon.(Need to duplicate from Default Weapon FX)
  3. Create a new GA_Glace_Death(Duplicate from GA_Guadrain_Death), change a new Nigara Disolve System.

- 4, ⚠️⚠️Ranged Behavior Tree + Debug :Rolling Issue
  1. Debugging rolling issue: When a character tries to roll toward the wall, it will roll back to its world's original position.
      In GA_Roll, we use a blocking hit to calculate the required destination, so we need to branch if there's no blocking hit, cause rolling does not finish while the character is facing to wall.\
     ![Screenshot 2025-05-20 113737](https://github.com/user-attachments/assets/00baf18d-0a28-4116-a8ba-2b677a81c199)
  2. Create a new child AIC_Galcer from AIC_Enemy_Base, and also duplicate the required Glacer Behavior Tree, attach it to AIC, and enable the BP_Glacer using this new AIC.
 
- 5, 




 



















  








  



  










  


  

  








  











  


































  




  

























  
  


  










  


  






