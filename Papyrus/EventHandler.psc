Scriptname KYF:EventHandler extends Quest

KYF:ActorValuesManager Property ActorValuesManager Auto Const

Book Property NPCInfoBook Auto Const
Activator Property NameHolder Auto Const

int keyPressCount = 0
int pressCounterResetTimer = 1 const
bool blocked = false
string controlKey = "Sprint" const
bool ll4pInstalled

Event OnInit()
	RegisterForRemoteEvent(Game.GetPlayer(), "OnPlayerLoadGame")
    EnsureRegistered()
EndEvent
Event Actor.OnPlayerLoadGame(Actor akSender)
    EnsureRegistered()
EndEvent

function EnsureRegistered()
    UnregisterForControl(controlKey)
    ll4pInstalled = false
    if LL_FourPlay.GetLLFPPluginVersion() > 0
        ll4pInstalled = true
    endif
    RegisterForControl(controlKey)
endfunction

Event OnControlUp(string control, float time)
    if blocked || control != controlKey || Game.getPlayer().IsInCombat()
        return
    endif
    if time < 0.2 ; short press
        CancelTimer(pressCounterResetTimer)
        keyPressCount += 1
        if keyPressCount >= 2
            blocked = true
            keyPressCount = 0
            ShowTargetInfo()
            blocked = false
        else
            ; it was single press
            ; forget about it after short interval
            StartTimer(0.3, pressCounterResetTimer)
        endif
    else
        keyPressCount = 0
    endif
EndEvent

Event OnTimer(Int TimerID)
    if pressCounterResetTimer == TimerID
        keyPressCount = 0
    endif
EndEvent

function ShowTargetInfo()
    Actor a
    Actor player = Game.GetPlayer()
    if player.IsInScene()
        a = player.GetDialogueTarget()
    endif
    if !a && ll4pInstalled
        a = LL_FourPlay.LastCrossHairRef() as Actor
    endif
    if a 
        DisplayInfo(a)
    endif
endfunction

function DisplayInfo(Actor a)
    if !ActorValuesManager
        Debug.Trace("KYF: missing data file?")
        return
    endif
    string content = ActorValuesManager.BuildDisplayString(a)
    ObjectReference myNote = Game.GetPlayer().PlaceAtMe(NPCInfoBook, abForcePersist = false, abInitiallyDisabled = false, abDeleteWhenAble = true)
    
    if !myNote
        Debug.Trace("KYF: failed to create note")
        return
    endif
    NameHolder.SetName(content)
    myNote.AddTextReplacementData("Content", NameHolder)
    myNote.AddTextReplacementData("Title", a.GetLeveledActorBase())
    
    if myNote.Activate(Game.GetPlayer())
        RegisterForMenuOpenCloseEvent("BookMenu")    
    endif
    myNote.Disable()
    myNote.Delete()
    
endfunction

Event OnMenuOpenCloseEvent(string asMenuName, bool abOpening)
    if asMenuName == "BookMenu"
        if !abOpening
            ; book closed somehow
            UnregisterForMenuOpenCloseEvent("BookMenu")
            Game.GetPlayer().RemoveItem(NPCInfoBook, 1, true) ; remove it if player got the book
        endif
    endif
EndEvent





