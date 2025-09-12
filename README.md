# Sorcery

## Code Highlight
Rotate the element wheel left and right with Q/E. Those inputs are stored in a queue that resets if you switch directions but stores input if you spam it.
```cpp
void ASorceryCharacter::QueueElementWheelLeft()
{
	int32* peek = EWRotationQueue.Peek();

	if (peek != nullptr && *peek == EWLeftRotationValue)
	{
		EWRotationQueue.Enqueue(EWLeftRotationValue);
	}
	else
	{
		EWRotationQueue.Empty();
		EWRotationQueue.Enqueue(EWLeftRotationValue);
		RotateElementWheel();
	}
}
```
```cpp
void ASorceryCharacter::QueueElementWheelRight()
{
	int32* peek = EWRotationQueue.Peek();
	if (peek != nullptr && *peek == EWRightRotationValue)
	{
		EWRotationQueue.Enqueue(EWRightRotationValue);
	}
	else
	{
		EWRotationQueue.Empty();
		EWRotationQueue.Enqueue(EWRightRotationValue);
		RotateElementWheel();
	}
}
```

The function "RotateElementalWheel" is implemented in blueprints:
<img width="1831" height="780" alt="Screenshot 2025-09-11 194822" src="https://github.com/user-attachments/assets/be453d35-f9b8-49eb-a368-aa2d27ab12c2" />

This function handles removing input from the queue and passing it along or not.
```cpp
bool ASorceryCharacter::ProcessElementWheelQueue()
{
	if (EWRotationQueue.IsEmpty())
		return false;

	EWRotationQueue.Dequeue(EWCurrentRotation);
	EWStartRotation = ElementWheel->GetRelativeRotation();
	EWPreviousRotation = EWStartRotation;
	return true;
}
```

This is what handles the actual rotation of the wheel taking input from the Timeline in the blueprint.
```cpp
void ASorceryCharacter::UpdateElementWheelRotation(float NormalizedRotation)
{
	FRotator Rotation = EWStartRotation;
	Rotation.Pitch = FMath::Lerp(EWStartRotation.Pitch, EWStartRotation.Pitch + EWCurrentRotation, NormalizedRotation);
	ElementWheel->AddLocalRotation(Rotation - EWPreviousRotation);
	EWPreviousRotation = Rotation;
}
```

This following function will need to be changed in the future.
There's a collider at the apex of the wheel and the element that overlaps with it dictates what element the fired projectile is.
```cpp
void ASorceryCharacter::ElementSelectOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// NOTE dirty implementation, dependent on static mesh, which will likely be switched out
	if (!OtherComp->ComponentHasTag(TEXT("ElementOrb")))
		return;

	UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(OtherComp);
	if (!StaticMeshComp)
		return;

	UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh();
	if (StaticMesh == SMFireElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Fire;
		SMFireElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMIceElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Ice;
		SMIceElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMShockElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Shock;
		SMShockElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
	else if (StaticMesh == SMAcidElement->GetStaticMesh())
	{
		ActiveElement = EElementalType::Acid;
		SMAcidElement->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
		UpdateActiveElementalType();
	}
}
```

Projectiles have functions to set their own element.
They will also handle other mnodifiers like exploding or chain lightning, etc.
```cpp
void ASorceryCharacter::UpdateActiveElementalType()
{
	ASorceryProjectile* projectile = Cast<ASorceryProjectile>(ProjectileClass->GetDefaultObject());
	if (projectile) projectile->ChangeElementalType(ActiveElement);
}
```
