// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FlockAI : ModuleRules
{
	public FlockAI(ReadOnlyTargetRules Target) : base(Target)
	{
        PrivatePCHHeaderFile = "Public/FlockAI.h";

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
