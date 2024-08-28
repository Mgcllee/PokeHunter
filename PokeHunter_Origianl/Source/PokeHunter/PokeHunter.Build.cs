// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PokeHunter : ModuleRules
{
	public PokeHunter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "GameplayTasks", "NavigationSystem", "Niagara",
                                                            "HeadMountedDisplay",
                                                            "GameLiftServerSDK", "UMG", "SlateCore", "HTTP", "Json", "JsonUtilities", "WebBrowserWidget", "Slate",
                                                            "Networking", "Sockets"});

        bEnableUndefinedIdentifierWarnings = false;
        PrivateDependencyModuleNames.Add("OnlineSubsystem");
    }
}
