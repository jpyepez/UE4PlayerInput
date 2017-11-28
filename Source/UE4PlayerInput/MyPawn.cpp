
#include "MyPawn.h"
#include "UE4PlayerInput.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Sets default values
AMyPawn::AMyPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    // Set this pawn to be controlled by the lowest-numbered player
    AutoPossessPlayer = EAutoReceiveInput::Player0;
    
    // Create a dummy root component we can attach things to.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    // Create a camera and a visible object
    UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
    OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
    // Attach our camera and visible object to our root component. Offset and rotate the camera.
    OurCamera->SetupAttachment(RootComponent);
    OurCamera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
    OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
    OurVisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentScale = OurVisibleComponent->GetComponentScale().X;
	if (bGrowing) {
		// Grow to double size over the course of one second
		CurrentScale += DeltaTime;
	}
	else {
		// Shrink half as fast as we grow
		CurrentScale -= (DeltaTime * 0.5f);
	}

	// Make sure we never drop below our starting size, or increase past double size.
	CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 2.0f);
	OurVisibleComponent->SetWorldScale3D(FVector(CurrentScale));

	// Handle movement basd on our "MoveX" and "MoveY" axes
	if (!CurrentVelocity.IsZero()) {

		// set as moving
		bMoving = true;

		// check and update counter
		if (bLastMoving) {
			moveCounter++;
			UE_LOG(LogTemp, Warning, TEXT("Movement counter is %d."), moveCounter);
		}
		
		// check if time threshold for fast speed has been reached
		if (moveCounter >= moveAfterFrames) {
			bFast = true;
		}
		else {
			bFast = false;
		}

		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
		SetActorLocation(NewLocation);

		// set last as moving
		bLastMoving = true;
	}
	else {
		bMoving = false;
		bLastMoving = false;
		bFast = false;
		moveCounter = 0;

	}
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Respond when our "Grow" key is pressed or released.
	InputComponent->BindAction("Grow", IE_Pressed, this, &AMyPawn::StartGrowing);
	InputComponent->BindAction("Grow", IE_Released, this, &AMyPawn::StopGrowing);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveX", this, &AMyPawn::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &AMyPawn::Move_YAxis);

}

void AMyPawn::Move_XAxis(float AxisValue)
{
	float speed;
	if (bFast) {
		speed = 150.0f;
	}
	else {
		speed = 50.0f;
	}

	// Move at 100 units per second forward or backward
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * speed;
}

void AMyPawn::Move_YAxis(float AxisValue)
{
	float speed;
	if (bFast) {
		speed = 150.0f;
	}
	else {
		speed = 50.0f;
	}

	// Move at 100 units per second right or left
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * speed;
}

void AMyPawn::StartGrowing()
{
	bGrowing = true;
}

void AMyPawn::StopGrowing()
{
	bGrowing = false;
}

