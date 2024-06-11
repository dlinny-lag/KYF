Scriptname KYF:ActorValuesManager extends Quest

Keyword Property MainStats Auto Const
Keyword Property Affinity Auto Const
Keyword Property Resists Auto Const

Event OnInit()
    Debug.Trace("KYF: OnInit")
	RegisterForRemoteEvent(Game.GetPlayer(), "OnPlayerLoadGame")
    InitAVs()
    EnsureInitialized()
EndEvent
Event Actor.OnPlayerLoadGame(Actor akSender)
    Debug.Trace("KYF: OnLoad")
    EnsureInitialized()
EndEvent

function EnsureInitialized()

endfunction


string modName = "Fallout4.esm" const


function InitAVs()
    
    KYF:Internal.UnregisterGroup(MainStats)
    KYF:Internal.UnregisterGroup(Affinity)
    KYF:Internal.UnregisterGroup(Resists)
    KYF:Internal.RegisterGroup(MainStats, 0)
    KYF:Internal.RegisterGroup(Affinity, 1)
    KYF:Internal.RegisterGroup(Resists, 2)
    KYF:Internal.SetGroupVisibility(MainStats, true)
    KYF:Internal.SetGroupVisibility(Affinity, true)
    KYF:Internal.SetGroupVisibility(Resists, true)

    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C9, modName) as ActorValue, MainStats, 0, false, 0, "Exp")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C2, modName) as ActorValue, MainStats, 1, true, 0, "Strength")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C3, modName) as ActorValue, MainStats, 2, true, 0, "Perception")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C4, modName) as ActorValue, MainStats, 3, true, 0, "Endurance")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C5, modName) as ActorValue, MainStats, 4, true, 0, "Charisma")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C6, modName) as ActorValue, MainStats, 5, true, 0, "Intellegence")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C7, modName) as ActorValue, MainStats, 6, true, 0, "Agility")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2C8, modName) as ActorValue, MainStats, 7, true, 0, "Luck")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2D4, modName) as ActorValue, MainStats, 8, true, 0, "Health")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2D5, modName) as ActorValue, MainStats, 9, true, 0, "AP")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2DC, modName) as ActorValue, MainStats, 10, false, 0, "Carry Weight")

    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0xA1B80, modName) as ActorValue, Affinity, 0, false, 0, "Affinity")

    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2E3, modName) as ActorValue, Resists, 0, false, 0, "Damage resist")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2E4, modName) as ActorValue, Resists, 1, false, 0, "Poison Resist")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2E5, modName) as ActorValue, Resists, 2, false, 0, "Fire Resist")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2E6, modName) as ActorValue, Resists, 3, false, 0, "Electric Resist")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2E7, modName) as ActorValue, Resists, 4, false, 0, "Frost Resist")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2EA, modName) as ActorValue, Resists, 5, false, 0, "Rad Resist")
    KYF:Internal.RegisterAttribute(Game.GetFormFromFile(0x2EB, modName) as ActorValue, Resists, 6, false, 0, "Energy Resist")

    ;KYF:Internal.Dump()
endfunction


string function BuildDisplayString(Actor a)
    string total = "\nLevel: " + a.GetLevel() + "\n"
    total += KYF:Internal.GetReportString(a)
    return total
endfunction