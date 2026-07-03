// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PP_1 : ModuleRules
{
	public PP_1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
    }
}
