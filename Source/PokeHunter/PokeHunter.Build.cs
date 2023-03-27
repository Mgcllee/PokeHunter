// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PokeHunter : ModuleRules
{
	public PokeHunter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "AIModule", "GameplayTasks", "NavigationSystem", "Niagara" });

        // "Networking", "Sockets", "HeadMountedDisplay", "GameLiftServerSDK", "SlateCore", "Json", "JsonUtilities", "WebBrowserWidget", "Http",
        //PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        bEnableUndefinedIdentifierWarnings = false;
        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
