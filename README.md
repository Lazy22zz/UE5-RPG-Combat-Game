# UE5-RPG-Combat-Game
 creating a combat action rpg game
# EXTRA
- 1, hard reference :\
  A hard reference is a direct pointer to another object or asset. When an object A hard-references object B, both objects are loaded into memory together. \
  pros:\
  Simplicity: Easier to work with since everything is loaded and accessible.\
  Guaranteed Availability: Ensures the referenced asset is loaded into memory when needed.\
  Fast Access: No need to check or load the referenced asset dynamically.\
  cons:\
  High Memory Usage: All referenced objects are loaded into memory, even if some are not needed immediately.\
  Longer Load Times: Increases initial loading time as all hard-referenced objects must be loaded upfront.\
  Potential for Circular Dependencies: Can lead to complex dependency chains, making debugging and optimization harder.\
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

  

  
