Scriptname KYF:Internal Native Hidden Const

string function GetVersionString() native global
int function GetVersionInt() native global

; registers group of attributes for displaying
; id - id of group, used in subsequent calls
; priority - order of group in final report. lower value - higher in report
; return:
;  true - on success
;  false - if group registered already
; NOTE: groups added as invisible, you have to make them visible explicitly
bool function RegisterGroup(Keyword id, int priority) native global

; removes group of attributes
; id - id of group to remove
; return:
;  true - on success
;  false - if group was not registered
bool function UnregisterGroup(Keyword id) native global

; regsters attribute in the group
; attribute - attribute to register
; groupId - id of group where this attribute will be placed
; indexInGroup - order of attribute within group. lower value - higher in report
; showBaseValue - is base value of attribute need to be displayed in addition to current value
; precision - how many decimal digits of attribute value (and base value) to display after decimal point
; defaultName - string that will be displayed to user if attribute hasn't any name to display
; return:
;  true - on success
;  false - attribute is None, group was not registered
; NOTE: you can register same attribute multiple times in a single group
bool function RegisterAttribute(ActorValue attribute, Keyword groupId, int indexInGroup, bool showBaseValue, int precision, string defaultName) native global

; removes first found attribute from the group
; attribute - attribute to remove
; groupId - id of group from which to remove the attribute
; return:
; true - if any attribute removed
; false - attribute is None, group was not registered, attribute was not found in the group
bool function UnregisterAttribute(ActorValue attribute, Keyword groupId) native global

; returns ordered list of registered groups
Keyword[] function GetGroups() native global

; returns ordered list of attributes registered in the group
; groupId - id of group whose attributes to return
; returns None of group was not registered
ActorValue[] function GetAttributes(Keyword groupId) native global

; enables or disables displaying of certain group
; attributes in invisible group are not appear in report
; groupId - id of group to set visibility flag
; visibilty - true to enable displayin, false - otherwise
; return:
; true - on success
; false - group was not registered
bool function SetGroupVisibility(Keyword groupId, bool visibility) native global

; returns visibility flag for certain group
; groupId - id of group to check visibility flag
; return:
; true - if group is visible, i.e. enabled for displaying in report
; false - group was not found, group is invisible
bool function GetGroupVisibility(Keyword groupId) native global

; returns attributes report for certain actor
; attributes appears in report according to order of groups and order of attributes within group
; attributes in invisible groups are not appear in report
; a - actor to generate report
; NOTE: empty string will be returned if actor is None or no groups/attributes was registered
string function GetReportString(Actor a) native global

; dumps registration info to log
function Dump() native global
