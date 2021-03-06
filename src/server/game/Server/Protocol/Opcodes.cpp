/*
 * Copyright (C) 2011-2014 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2014 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Opcodes.h"
#include "WorldSession.h"

OpcodeTable serverOpcodeTable;
OpcodeTable clientOpcodeTable;

template<bool isInValidRange, bool isNonZero>
void OpcodeTable::ValidateAndSetOpcode(uint16 /*opcode*/, uint16 /*opcodeNumber*/, char const* /*name*/, SessionStatus /*status*/, PacketProcessing /*processing*/, pOpcodeHandler /*handler*/)
{
    // if for some reason we are here, that means NUM_OPCODE_HANDLERS == 0 (or your compiler is broken)
}

template<>
void OpcodeTable::ValidateAndSetOpcode<true, true>(uint16 opcode, uint16 opcodeNumber, char const* name, SessionStatus status, PacketProcessing processing, pOpcodeHandler handler)
{
    if (_internalTable[opcode] != NULL)
    {
        TC_LOG_ERROR("network", "Tried to override handler of %s with %s (opcode %u)", _internalTable[opcode]->Name, name, opcodeNumber);
        return;
    }

    _internalTable[opcode] = new OpcodeHandler(opcodeNumber, name, status, processing, handler);
    _opcodeTable[opcodeNumber] = (Opcodes)opcode;
}

template<>
void OpcodeTable::ValidateAndSetOpcode<false, true>(uint16 /*opcode*/, uint16 opcodeNumber, char const* /*name*/, SessionStatus /*status*/, PacketProcessing /*processing*/, pOpcodeHandler /*handler*/)
{
    TC_LOG_ERROR("network", "Tried to set handler for an invalid opcode %d", opcodeNumber);
}

template<>
void OpcodeTable::ValidateAndSetOpcode<true, false>(uint16 /*opcode*/, uint16 /*opcodeNumber*/, char const* name, SessionStatus /*status*/, PacketProcessing /*processing*/, pOpcodeHandler /*handler*/)
{
    TC_LOG_ERROR("network", "Opcode %s got value 0", name);
}

/// Correspondence between opcodes and their names
void OpcodeTable::InitializeClientTable()
{
#define DEFINE_OPCODE_HANDLER(opcode, opcodeNumber, status, processing, handler)                                      \
    ValidateAndSetOpcode<(opcodeNumber < NUM_OPCODE_HANDLERS), (opcode != 0)>(opcode, opcodeNumber, #opcode, status, processing, handler);

    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_LEVEL_GRANT,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAcceptGrantLevel             );
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_TRADE,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAcceptTradeOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_ACTIVATETAXI,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleActivateTaxiOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_ACTIVATETAXIEXPRESS,                                 0x1B52, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleActivateTaxiExpressOpcode    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ADDON_REGISTERED_PREFIXES,                           0x03F4, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddonRegisteredPrefixesOpcode); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ADD_FRIEND,                                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddFriendOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_ADD_IGNORE,                                          0x1321, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAddIgnoreOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ALTER_APPEARANCE,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAlterAppearance              );
    DEFINE_OPCODE_HANDLER(CMSG_AREATRIGGER,                                         0x01B4, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAreaTriggerOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUERY,                            0x0A08, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAreaSpiritHealerQueryOpcode  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUEUE,                            0x1921, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAreaSpiritHealerQueueOpcode  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ATTACKSTOP,                                          0x065B, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleAttackStopOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ATTACKSWING,                                         0x1E9B, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleAttackSwingOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUTH_SESSION,                                        0x0487, STATUS_NEVER,     PROCESS_THREADUNSAFE, &WorldSession::Handle_EarlyProccess               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BANKER_ACTIVATE,                                     0x1B24, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBankerActivateOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BEGIN_TRADE,                                         0x06CC, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBeginTradeOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BINDER_ACTIVATE,                                     0x02F3, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBinderActivateOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BUG,                                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleBugOpcode                    );
    DEFINE_OPCODE_HANDLER(CMSG_BUSY_TRADE,                                          0x00D5, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBusyTradeOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BUY_BANK_SLOT,                                       0x0A25, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBuyBankSlotOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_AURA,                                         0x08AE, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCancelAuraOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_AUTO_REPEAT_SPELL,                            0x0939, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCancelAutoRepeatSpellOpcode  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_CAST,                                         0x0071, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleCancelCastOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_CHANNELLING,                                  0x0BE6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCancelChanneling             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_MOUNT_AURA,                                   0x03E6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCancelMountAuraOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_QUEUED_SPELL,                                 0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_TEMP_ENCHANTMENT,                             0x13C4, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCancelTempEnchantmentOpcode  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_TRADE,                                        0x1159, STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT, PROCESS_THREADUNSAFE, &WorldSession::HandleCancelTradeOpcode); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CAST_SPELL,                                          0x08FE, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleCastSpellOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CHANGEPLAYER_DIFFICULTY,                             0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE,                  0x0581, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleChangeSeatsOnControlledVehicle); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_CREATE,                                         0x070D, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleCharCreateOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_CUSTOMIZE,                                      0x072A, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleCharCustomize                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_DELETE,                                         0x030E, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleCharDeleteOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_ENUM,                                           0x0918, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleCharEnumOpcode               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_FACTION_CHANGE,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCharFactionOrRaceChange      );
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_RACE_CHANGE,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCharFactionOrRaceChange      );
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_RENAME,                                         0x038B, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleCharRenameOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_UNDELETE,                                       0x0D99, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCharUndeleteOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_UNDELETE_COOLDOWN,                              0x19A9, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleCharUndeleteCooldownOpcode   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CLEAR_RAID_MARKER,                                   0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_ENABLE,                                  0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_ENTER_INSTANCE,                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_EXIT_INSTANCE,                           0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_GET_MAP_INFO,                            0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_GET_PARTY_INFO,                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_GET_PLAYER_INFO,                         0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_INSTANCE_COMMAND,                        0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_SKIRMISH_QUEUE_COMMAND,                  0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMMENTATOR_START_WARGAME,                           0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMPLAIN,                                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleComplainOpcode               );
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_CINEMATIC,                                  0x1B51, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCompleteCinematic            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_MOVIE,                                      0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CONNECT_TO_FAILED,                                   0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CONTACT_LIST,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleContactListOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_CORPSE_MAP_POSITION_QUERY,                           0x13AB, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCorpseMapPositionQuery       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CREATURE_QUERY,                                      0x0505, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleCreatureQueryOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_DANCE_QUERY,                                         0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_DB_QUERY_BULK,                                       0x138B, STATUS_AUTHED,    PROCESS_INPLACE,      &WorldSession::HandleDBQueryBulk                  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_DEL_FRIEND,                                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleDelFriendOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_DEL_IGNORE,                                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleDelIgnoreOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_DISMISS_CONTROLLED_VEHICLE,                          0x020A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleDismissControlledVehicle     ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_DISMISS_CRITTER,                                     0x02F1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleDismissCritter               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_DUEL_PROPOSED,                                       0x0B25, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleDuelProposedOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_DUEL_RESPONSE,                                       0x1BD2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleDuelResponseOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_EJECT_PASSENGER,                                     0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_EMOTE,                                               0x0A27, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleEmoteOpcode                  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ENABLETAXI,                                          0x0B91, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleTaxiQueryAvailableNodes      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ENABLE_NAGLE,                                        0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_EarlyProccess               );
    DEFINE_OPCODE_HANDLER(CMSG_EQUIPMENT_SET_DELETE,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleEquipmentSetDelete           );
    DEFINE_OPCODE_HANDLER(CMSG_EQUIPMENT_SET_SAVE,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleEquipmentSetSave             );
    DEFINE_OPCODE_HANDLER(CMSG_EQUIPMENT_SET_USE,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleEquipmentSetUse              );
    DEFINE_OPCODE_HANDLER(CMSG_FAR_SIGHT,                                           0x1C8A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleFarSightOpcode               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJECT_QUERY,                                    0x03AE, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleGameObjectQueryOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJ_REPORT_USE,                                  0x082E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGameobjectReportUse          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJ_USE,                                         0x0B19, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGameObjectUseOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GDF_SIM_CHEAT,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SHOW_ADVERTS_CHEAT,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SHOW_BRAIN,                                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GET_TROPHY_LIST,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_REPLACE_TROPHY,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GET_MAIL_LIST,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGetMailList                  );
    DEFINE_OPCODE_HANDLER(CMSG_GET_MIRRORIMAGE_DATA,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMirrorImageDataRequest       );
    DEFINE_OPCODE_HANDLER(CMSG_GM_REPORT_LAG,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleReportLag                    );
    DEFINE_OPCODE_HANDLER(CMSG_GOSSIP_HELLO,                                        0x00F3, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGossipHelloOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GOSSIP_SELECT_OPTION,                                0x0332, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGossipSelectOptionOpcode     ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GRANT_LEVEL,                                         0x0BE3, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGrantLevel                   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_HEARTH_AND_RESURRECT,                                0x0930, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleHearthAndResurrect           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_IGNORE_TRADE,                                        0x1419, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleIgnoreTradeOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_INITIATE_TRADE,                                      0x19BB, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleInitiateTradeOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_INSPECT,                                             0x01EC, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleInspectOpcode                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_INSPECT_HONOR_STATS,                                 0x09FD, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleInspectHonorStatsOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_INSTANCE_LOCK_WARNING_RESPONSE,                      0x1323, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleInstanceLockResponse         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_KEEP_ALIVE,                                          0x0981, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_EarlyProccess               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LEARN_TALENT,                                        0x0BB6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLearnTalentOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LIST_INVENTORY,                                      0x0B39, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleListInventoryOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LOAD_SCREEN,                                         0x0B08, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleLoadScreenOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LOGOUT_CANCEL,                                       0x03C2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLogoutCancelOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LOGOUT_REQUEST,                                      0x1911, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLogoutRequestOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LOG_DISCONNECT,                                      0x04D5, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::Handle_EarlyProccess               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_CREATE_TEXT_ITEM,                               0x13D1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMailCreateTextItem           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_DELETE,                                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMailDelete                   );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_MARK_AS_READ,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMailMarkAsRead               );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_RETURN_TO_SENDER,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMailReturnToSender           );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_TAKE_ITEM,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMailTakeItem                 );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_TAKE_MONEY,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMailTakeMoney                );
    DEFINE_OPCODE_HANDLER(CMSG_MINIGAME_MOVE,                                       0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_MINIMAP_PING,                                        0x0116, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMinimapPingOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOUNTSPECIAL_ANIM,                                   0x06DB, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMountSpecialAnimOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_NAME_QUERY,                                          0x0B15, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleNameQueryOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REALM_NAME_QUERY,                                    0x1189, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleRealmNameQueryOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_NEXT_CINEMATIC_CAMERA,                               0x1B3A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleNextCinematicCamera          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_NPC_TEXT_QUERY,                                      0x0ECB, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleNpcTextQueryOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_OBJECT_UPDATE_FAILED,                                0x14E2, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleObjectUpdateFailedOpcode     ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_OBJECT_UPDATE_RESCUED,                               0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_OFFER_PETITION,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleOfferPetitionOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_OPENING_CINEMATIC,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleOpeningCinematic             );
    DEFINE_OPCODE_HANDLER(CMSG_OPT_OUT_OF_LOOT,                                     0x0810, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleOptOutOfLootOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PAGE_TEXT_QUERY,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePageTextQueryOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_SILENCE,                                       0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_UNSILENCE,                                     0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_BUY,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetitionBuyOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_DECLINE,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetitionDeclineOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_RENAME,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetitionRenameOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_QUERY,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetitionQueryOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SHOWLIST,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetitionShowListOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SHOW_SIGNATURES,                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetitionShowSignOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SIGN,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetitionSignOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_PING,                                                0x0416, STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_EarlyProccess               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PLAYED_TIME,                                         0x1BB2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePlayedTime                   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PLAYER_LOGIN,                                        0x0B1D, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandlePlayerLoginOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PLAYER_VEHICLE_ENTER,                                0x11BC, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleEnterPlayerVehicle           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PLAY_DANCE,                                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_PUSHQUESTTOPARTY,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePushQuestToParty             );
    DEFINE_OPCODE_HANDLER(CMSG_PVP_LOG_DATA,                                        0x08B5, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandlePVPLogDataOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_RAID_READY_CHECK,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleRaidReadyCheckOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_RAID_READY_CHECK_CONFIRM,                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleRaidReadyCheckConfirmOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_RANDOM_ROLL,                                         0x12AA, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRandomRollOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_RANDOMIZE_CHAR_NAME,                                 0x1981, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleRandomizeCharNameOpcode      ); // 6.0.3 19116
    //DEFINE_OPCODE_HANDLER(CMSG_READY_FOR_ACCOUNT_DATA_TIMES,                        0x0000, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleReadyForAccountDataTimes   );
    DEFINE_OPCODE_HANDLER(CMSG_REALM_SPLIT,                                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleRealmSplitOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_RECLAIM_CORPSE,                                      0x1BC4, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleReclaimCorpseOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REDIRECTION_AUTH_PROOF,                              0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_REORDER_CHARACTERS,                                  0x0DAA, STATUS_AUTHED,    PROCESS_INPLACE,      &WorldSession::HandleReorderCharacters            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REPOP_REQUEST,                                       0x1BC2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRepopRequestOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REPORT_PVP_AFK,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleReportPvPAFK                 );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_ACCOUNT_DATA,                                0x03DE, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRequestAccountData           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CATEGORY_COOLDOWNS,                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleRequestCategoryCooldowns     );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CEMETERY_LIST,                               0x10A2, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleRequestCemeteryList          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_HOTFIX,                                      0x09AC, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_INSPECT_RATED_BG_STATS,                      0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PARTY_MEMBER_STATS,                          0x031D, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRequestPartyMemberStatsOpcode); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PVP_OPTIONS_ENABLED,                         0x120C, STATUS_AUTHED,    PROCESS_INPLACE,      &WorldSession::HandleRequestPvpOptions            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PVP_REWARDS,                                 0x039B, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleRequestPvpReward             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RAID_INFO,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleRequestRaidInfoOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RATED_INFO,                                  0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleRequestRatedInfo             );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RESEARCH_HISTORY,                            0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_EXIT,                                0x09CE, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRequestVehicleExit           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_NEXT_SEAT,                           0x1672, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleChangeSeatsOnControlledVehicle); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_PREV_SEAT,                           0x0527, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleChangeSeatsOnControlledVehicle); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_SWITCH_SEAT,                         0x0AED, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleChangeSeatsOnControlledVehicle); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_RESET_FACTION_CHEAT,                                 0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_RESET_INSTANCES,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleResetInstancesOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_RESURRECT_RESPONSE,                                  0x073E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleResurrectResponseOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_RETURN_TO_GRAVEYARD,                                 0x0D3E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleReturnToGraveyard            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ROLE_POLL_BEGIN,                                     0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SAVE_CUF_PROFILES,                                   0x0053, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleSaveCUFProfiles              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SELF_RES,                                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSelfResOpcode                );
    DEFINE_OPCODE_HANDLER(CMSG_SEND_MAIL,                                           0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSendMail                     );
    DEFINE_OPCODE_HANDLER(CMSG_SEND_SOR_REQUEST_VIA_ADDRESS,                        0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SEND_SOR_REQUEST_VIA_BNET_ACCOUNT_ID,                0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SETSHEATHED,                                         0x1B43, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleSetSheathedOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTIONBAR_TOGGLES,                               0x0BD2, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleSetActionBarToggles          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTION_BUTTON,                                   0x0599, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSetActionButtonOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_ALLOW_LOW_LEVEL_RAID1,                           0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_ALLOW_LOW_LEVEL_RAID2,                           0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_CONTACT_NOTES,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetContactNotesOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_CURRENCY_FLAGS,                                  0x14A1, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleChangeCurrencyFlags          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_DUNGEON_DIFFICULTY,                              0x012F, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSetDungeonDifficultyOpcode   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_EVERYONE_IS_ASSISTANT,                           0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_ATWAR,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetFactionAtWar              );
    DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_INACTIVE,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetFactionInactiveOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_SET_PLAYER_DECLINED_NAMES,                           0x018B, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSetPlayerDeclinedNames       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_PREFERED_CEMETERY,                               0x1461, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleSetPreferedCemetery          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_SPECIALIZATION,                                  0x0AC5, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandeSetTalentSpecialization       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_PVP,                                             0x19B9, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleSetPvP                       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_RELATIVE_POSITION,                               0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_SAVED_INSTANCE_EXTEND,                           0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetSavedInstanceExtend       );
    DEFINE_OPCODE_HANDLER(CMSG_SET_SELECTION,                                       0x05BD, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleSetSelectionOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_TAXI_BENCHMARK_MODE,                             0x1332, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSetTaxiBenchmarkOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_TITLE,                                           0x0BC6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSetTitleOpcode               );
    DEFINE_OPCODE_HANDLER(CMSG_SET_VEHICLE_REC_ID_ACK,                              0x1973, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_WATCHED_FACTION,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetWatchedFactionOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_SHOWING_CLOAK,                                       0x0132, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleShowingCloakOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SHOWING_HELM,                                        0x11E1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleShowingHelmOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SOCKET_GEMS,                                         0x1BE1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSocketOpcode                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SORT_BAGS,                                           0x1399, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSortBagsOpcode               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SORT_BANK_BAGS,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSortBankBagsOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_SPELLCLICK,                                          0x0BC2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSpellClick                   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SPIRIT_HEALER_ACTIVATE,                              0x03EC, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSpiritHealerActivateOpcode   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_STANDSTATECHANGE,                                    0x0ABD, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleStandStateChangeOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_STOP_DANCE,                                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SUBMIT_BUG,                                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SUBMIT_COMPLAIN,                                     0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SUGGESTION_SUBMIT,                                   0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SUMMON_RESPONSE,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSummonResponseOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_SUSPEND_TOKEN,                                       0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SYNC_DANCE,                                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_TAXINODE_STATUS_QUERY,                               0x1BC1, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleTaxiNodeStatusQueryOpcode    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TAXIQUERYAVAILABLENODES,                             0x000E, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleTaxiQueryAvailableNodes      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TELEPORT_TO_UNIT,                                    0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_TEXT_EMOTE,                                          0x0518, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTextEmoteOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TIME_ADJUSTMENT_RESPONSE,                            0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_TIME_SYNC_RESP,                                      0x0003, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleTimeSyncResp                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TIME_SYNC_RESP_FAILED,                               0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_TOGGLE_PVP,                                          0x1433, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTogglePvP                    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TOTEM_DESTROYED,                                     0x000D, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTotemDestroyed               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TRAINER_BUY_SPELL,                                   0x0A28, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTrainerBuySpellOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TRAINER_LIST,                                        0x0A2E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTrainerListOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_TURN_IN_PETITION,                                    0x0000, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTurnInPetitionOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_TUTORIAL_FLAG,                                       0x0B16, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTutorialFlag                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_UNACCEPT_TRADE,                                      0x13BA, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUnacceptTradeOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_SKILL,                                       0x0A26, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUnlearnSkillOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_SPECIALIZATION,                              0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_UNREGISTER_ALL_ADDON_PREFIXES,                       0x082A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUnregisterAddonPrefixesOpcode); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_ACCOUNT_DATA,                                 0x12A1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUpdateAccountData            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_MISSILE_TRAJECTORY,                           0x1CDB, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleUpdateMissileTrajectory      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_PROJECTILE_POSITION,                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleUpdateProjectilePosition     );
    DEFINE_OPCODE_HANDLER(CMSG_USED_FOLLOW,                                         0x1609, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_VIOLENCE_LEVEL,                                      0x098D, STATUS_AUTHED,    PROCESS_INPLACE,      &WorldSession::HandleViolenceLevel                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_WARDEN_DATA,                                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleWardenDataOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_WARGAME_REQUEST,                                     0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleWargameRequest               );
    DEFINE_OPCODE_HANDLER(CMSG_WHO,                                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleWhoOpcode                    );
    DEFINE_OPCODE_HANDLER(CMSG_WHOIS,                                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleWhoisOpcode                  );
    DEFINE_OPCODE_HANDLER(CMSG_WORLD_STATE_UI_TIMER_UPDATE,                         0x0302, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleWorldStateUITimerUpdate      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_WORLD_TELEPORT,                                      0x038A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleWorldTeleportOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ZONEUPDATE,                                          0x0000, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleZoneUpdateOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_CORPSE_QUERY,                                        0x128A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCorpseQueryOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(MSG_LIST_STABLED_PETS,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleListStabledPetsOpcode        );
    DEFINE_OPCODE_HANDLER(MSG_NOTIFY_PARTY_SQUELCH,                                 0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(MSG_PARTY_ASSIGNMENT,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePartyAssignmentOpcode        );
    DEFINE_OPCODE_HANDLER(MSG_QUERY_NEXT_MAIL_TIME,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleQueryNextMailTime            );
    DEFINE_OPCODE_HANDLER(MSG_RAID_READY_CHECK_FINISHED,                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleRaidReadyCheckFinishedOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_RAID_TARGET_UPDATE,                                  0x1122, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRaidTargetUpdateOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_RAID_DIFFICULTY,                                 0x1121, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetRaidDifficultyOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_TABARDVENDOR_ACTIVATE,                               0x08EE, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTabardVendorActivateOpcode   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_CONFIRM_RESPEC_WIPE,                                 0x14B4, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleTalentWipeConfirmOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(MSG_VERIFY_CONNECTIVITY,                                  0x4F57, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_EarlyProccess               ); // Not an opcode

    ///< Items
    DEFINE_OPCODE_HANDLER(CMSG_TRANSMOGRIFY_ITEMS,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleTransmogrifyItems            );
    DEFINE_OPCODE_HANDLER(CMSG_AUTOBANK_ITEM,                                       0x0751, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAutoBankItemOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUTOEQUIP_ITEM,                                      0x0F35, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAutoEquipItemOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUTOEQUIP_ITEM_SLOT,                                 0x07B1, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAutoEquipItemSlotOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_BAG_ITEM,                                  0x0F18, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAutoStoreBagItemOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_BANK_ITEM,                                 0x0732, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAutoStoreBankItemOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_LOOT_ITEM,                                 0x0609, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAutostoreLootItemOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BUY_ITEM,                                            0x0352, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBuyItemOpcode                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BUYBACK_ITEM,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleBuybackItem                  );
    DEFINE_OPCODE_HANDLER(CMSG_DESTROY_ITEM,                                        0x01EE, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleDestroyItemOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_REFUND,                                         0x1111, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleItemRefund                   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_REFUND_INFO,                                    0x0924, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleItemRefundInfoRequest        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_TEXT_QUERY,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleItemTextQuery                );
    DEFINE_OPCODE_HANDLER(CMSG_OPEN_ITEM,                                           0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleOpenItemOpcode               );
    DEFINE_OPCODE_HANDLER(CMSG_READ_ITEM,                                           0x08F6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleReadItem                     ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REPAIR_ITEM,                                         0x0B54, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRepairItemOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SELL_ITEM,                                           0x0351, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSellItemOpcode               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SPLIT_ITEM,                                          0x0795, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSplitItemOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_SWAP_INV_ITEM,                                       0x0F17, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSwapInvItemOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SWAP_ITEM,                                           0x0736, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSwapItem                     ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_USE_ITEM,                                            0x08B6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUseItemOpcode                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_WRAP_ITEM,                                           0x0797, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleWrapItemOpcode               ); // 6.0.3 19116

    ///< Achivement
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_INSPECT_ACHIEVEMENTS,                          0x0A07, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQueryInspectAchievements     ); // 6.0.3 19116

    ///< Trade
    DEFINE_OPCODE_HANDLER(CMSG_CLEAR_TRADE_ITEM,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleClearTradeItemOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_CURRENCY,                                  0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_GOLD,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetTradeGoldOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_ITEM,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetTradeItemOpcode           );

    ///< Game master
    DEFINE_OPCODE_HANDLER(CMSG_GMRESPONSE_RESOLVE,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGMResponseResolve            );
    DEFINE_OPCODE_HANDLER(CMSG_GMSURVEY_SUBMIT,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGMSurveySubmit               );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_CREATE,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGMTicketCreateOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_DELETETICKET,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGMTicketDeleteOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_GETTICKET,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGMTicketGetTicketOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_SYSTEMSTATUS,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGMTicketSystemStatusOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_UPDATETEXT,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGMTicketUpdateOpcode         );

    ///< Void Storage
    DEFINE_OPCODE_HANDLER(CMSG_VOID_STORAGE_QUERY,                                  0x019E, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleVoidStorageQuery             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_VOID_STORAGE_TRANSFER,                               0x0463, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleVoidStorageTransfer          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_VOID_STORAGE_UNLOCK,                                 0x13BB, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleVoidStorageUnlock            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_VOID_SWAP_ITEM,                                      0x0619, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleVoidSwapItem                 ); // 6.0.3 19116

    ///< Battlefield & Battlegroud & Battlemaster
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_LEAVE,                                   0x06F1, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleBattlefieldLeaveOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_LIST,                                    0x03B1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlefieldListOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE,               0x059A, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleBfEntryInviteResponse        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_EXIT_REQUEST,                        0x0B49, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleBfExitRequest                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_QUEUE_INVITE_RESPONSE,               0x122A, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleBfQueueInviteResponse        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_QUEUE_REQUEST,                       0x1381, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_PORT,                                    0x11EB, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattleFieldPortOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_STATUS,                                  0x0320, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlefieldStatusOpcode      ); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(CMSG_BATTLEGROUND_PLAYER_POSITIONS,                       0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleBattlegroundPlayerPositionsOpcode);
    
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN,                                   0x0D2E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlemasterJoinOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN_ARENA,                             0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleBattlemasterJoinArena        );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN_RATED,                             0x06E4, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116

    ///< Auction & Black Market
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_HELLO,                                       0x13EA, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionHelloOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_BIDDER_ITEMS,                           0x050E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionListBidderItems       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_ITEMS,                                  0x0B53, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionListItems             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_OWNER_ITEMS,                            0x03E2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionListOwnerItems        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_PENDING_SALES,                          0x11E2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionListPendingSales      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_PLACE_BID,                                   0x0B11, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionPlaceBid              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_REMOVE_ITEM,                                 0x1BEB, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionRemoveItem            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_SELL_ITEM,                                   0x0027, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAuctionSellItem              ); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(CMSG_BLACKMARKET_BID,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleBlackMarketBidOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_BLACKMARKET_HELLO,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleBlackMarketHelloOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_BLACKMARKET_REQUEST_ITEMS,                           0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleBlackMarketRequestItemOpcode );

    ///< Group
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_ASSISTANT_LEADER,                              0x0B26, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGroupAssistantLeaderOpcode   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_CHANGE_SUB_GROUP,                              0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGroupChangeSubGroupOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_DISBAND,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGroupDisbandOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_INVITE,                                        0x122C, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGroupInviteOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_INVITE_RESPONSE,                               0x118A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGroupInviteResponseOpcode    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_RAID_CONVERT,                                  0x0137, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGroupRaidConvertOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_REQUEST_JOIN_UPDATES,                          0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_SET_LEADER,                                    0x091E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGroupSetLeaderOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_SET_ROLES,                                     0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleGroupSetRolesOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_SWAP_SUB_GROUP,                                0x0BA4, STATUS_NEVER,     PROCESS_THREADUNSAFE, &WorldSession::HandleGroupSwapSubGroupOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_UNINVITE_GUID,                                 0x1982, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGroupUninviteGuidOpcode      ); // 6.0.3 19116

    ///< LFG
    DEFINE_OPCODE_HANDLER(CMSG_LFG_GET_STATUS,                                      0x1BA4, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleLfgGetStatus                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_GET_SYSTEM_INFO,                                 0x030F, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLfgGetSystemInfo             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_JOIN,                                            0x0925, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLfgJoinOpcode                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LEAVE,                                           0x01A2, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleLfgLeaveOpcode               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_GET_STATUS,                                 0x0B17, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_JOIN,                                       0x0B84, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_LEAVE,                                      0x0DA9, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_UPDATE_REQUEST,                             0x07BD, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LOCK_INFO_REQUEST,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleLfgGetLockInfoOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_PROPOSAL_RESULT,                                 0x0509, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLfgProposalResultOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_SET_BOOT_VOTE,                                   0x0308, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleLfgSetBootVoteOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_SET_COMMENT,                                     0x0B89, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_SET_ROLES,                                       0x1B82, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleLfgSetRolesOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LFG_TELEPORT,                                        0x0316, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLfgTeleportOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_LFG_LIST_BLACKLIST,                          0x149B, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_LFG_FAST_LAUNCH,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_TIME_WALKER_DUNGEON,                             0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116

    ///< Chat & Voice Chat & Channel
    DEFINE_OPCODE_HANDLER(CMSG_ADD_VOICE_IGNORE,                                    0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_FILTERED,                                       0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_DEL_VOICE_IGNORE,                                    0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_VOICE_SESSION_ENABLE,                                0x1102, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleVoiceSessionEnableOpcode     ); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(CMSG_CHAT_IGNORED,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChatIgnoredOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_ADDON_BATTLEGROUND,                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddonMessagechatOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_ADDON_GUILD,                             0x1823, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddonMessagechatOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_ADDON_OFFICER,                           0x188A, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddonMessagechatOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_ADDON_PARTY,                             0x137C, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddonMessagechatOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_ADDON_RAID,                              0x082C, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddonMessagechatOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_ADDON_WHISPER,                           0x13DC, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleAddonMessagechatOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_AFK,                                     0x1BDC, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_BATTLEGROUND,                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_CHANNEL,                                 0x0913, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_DND,                                     0x0AAB, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_EMOTE,                                   0x113C, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_GUILD,                                   0x0B1B, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_OFFICER,                                 0x1884, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_PARTY,                                   0x0134, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_RAID,                                    0x1161, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_RAID_WARNING,                            0x0134, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_SAY,                                     0x1884, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_WHISPER,                                 0x0913, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MESSAGECHAT_YELL,                                    0x1161, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleMessagechatOpcode            ); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(CMSG_CLEAR_CHANNEL_WATCH,                                 0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_ANNOUNCEMENTS,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelAnnouncements         );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_BAN,                                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelBan                   );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_DISPLAY_LIST,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelDisplayListQuery      );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_INVITE,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelInvite                );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_KICK,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelKick                  );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_LIST,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelList                  );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_MODERATE,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_MODERATOR,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelModerator             );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_MUTE,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelMute                  );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_OWNER,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelOwner                 );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_PASSWORD,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelPassword              );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_ROSTER_INFO,                                 0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_SET_OWNER,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelSetOwner              );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_SILENCE_ALL,                                 0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_SILENCE_VOICE,                               0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNBAN,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelUnban                 );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNMODERATOR,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelUnmoderator           );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNMUTE,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelUnmute                );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNSILENCE_ALL,                               0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNSILENCE_VOICE,                             0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_VOICE_OFF,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelVoiceOnOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_VOICE_ON,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleChannelVoiceOnOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_JOIN_CHANNEL,                                        0x0B4B, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleJoinChannel                  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LEAVE_CHANNEL,                                       0x19F2, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleLeaveChannel                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_CHANNEL_WATCH,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetChannelWatch              );
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTIVE_VOICE_CHANNEL,                            0x031E, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetActiveVoiceChannel        ); // 6.0.3 19116

    ///< Loot
    DEFINE_OPCODE_HANDLER(CMSG_LOOT,                                                0x01B1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLootOpcode                   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_CURRENCY,                                       0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_MASTER_GIVE,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleLootMasterGiveOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_METHOD,                                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleLootMethodOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_MONEY,                                          0x1991, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLootMoneyOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_RELEASE,                                        0x1199, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLootReleaseOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_ROLL,                                           0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleLootRoll                     );

    ///< Battle Pet & Pet
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_DELETE_PET,                               0x0928, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlePetDelete              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_MODIFY_NAME,                              0x03A9, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlePetModifyName          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_QUERY_NAME,                               0x041C, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlePetQueryName           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_SET_BATTLE_SLOT,                          0x120B, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlePetSetBattleSlot       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_SET_FLAGS,                                0x0BA2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlePetSetFlags            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PET_SUMMON_COMPANION,                         0x0B38, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlePetSummonCompanion     ); // 6.0.3 19116
   
    DEFINE_OPCODE_HANDLER(CMSG_LEARN_PET_SPECIALIZATION_GROUP,                      0x08D6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandeLearnPetSpecializationGroup   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_ABANDON,                                         0x0005, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetAbandon                   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_ACTION,                                          0x133A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetAction                    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_CANCEL_AURA,                                     0x0A30, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetCancelAuraOpcode          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_CAST_SPELL,                                      0x0286, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetCastSpellOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_NAME_QUERY,                                      0x1433, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetNameQuery                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_RENAME,                                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandlePetRename                    );
    DEFINE_OPCODE_HANDLER(CMSG_PET_SET_ACTION,                                      0x020F, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetSetAction                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_SPELL_AUTOCAST,                                  0x0A2F, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetSpellAutocastOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_PET_STOP_ATTACK,                                     0x19E2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePetStopAttack                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PET_INFO,                                    0x0E37, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRequestPetInfoOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_PET_SLOT,                                        0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    
    ///< Quest
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_QUESTS_COMPLETED,                              0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleQueryQuestsCompleted         );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_TIME,                                          0x1489, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQueryTimeOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_CONFIRM_ACCEPT,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleQuestConfirmAccept           );
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_NPC_QUERY,                                     0x0000, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_POI_QUERY,                                     0x0B81, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestPOIQuery                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_QUERY,                                         0x09A6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestQueryOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_ACCEPT_QUEST,                             0x13D3, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestgiverAcceptQuestOpcode  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_CHOOSE_REWARD,                            0x0DAD, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestgiverChooseRewardOpcode ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_COMPLETE_QUEST,                           0x1E8A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestgiverCompleteQuest      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_HELLO,                                    0x058E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestgiverHelloOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_QUERY_QUEST,                              0x1924, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestgiverQueryQuestOpcode   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_REQUEST_REWARD,                           0x022E, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestgiverRequestRewardOpcode); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_STATUS_MULTIPLE_QUERY,                    0x0131, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestgiverStatusMultipleQuery); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_STATUS_QUERY,                             0x01E2, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleQuestgiverStatusQueryOpcode  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUESTLOG_REMOVE_QUEST,                               0x0139, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestLogRemoveQuest          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_PUSH_RESULT,                                   0x03D2, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQuestPushResult              ); // 6.0.3 19116  

    ///< Guild
    DEFINE_OPCODE_HANDLER(CMSG_AUTO_DECLINE_GUILD_INVITES,                          0x148A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAutoDeclineGuildInvites      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ACCEPT,                                        0x0B20, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildAcceptOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ACHIEVEMENT_MEMBERS,                           0x0C40, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ACHIEVEMENT_PROGRESS_QUERY,                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildAchievementProgressQuery);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ADD_RANK,                                      0x1630, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildAddRankOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ASSIGN_MEMBER_RANK,                            0x0436, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildAssignRankOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_BUY_TAB,                                  0x1BC3, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankBuyTab              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_DEPOSIT_MONEY,                            0x1B11, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankDepositMoney        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_LOG_QUERY,                                0x0428, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankLogQuery            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_MONEY_WITHDRAWN_QUERY,                    0x063D, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankMoneyWithdrawn      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_QUERY_TAB,                                0x1139, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankQueryTab            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_QUERY_TEXT,                               0x062D, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleQueryGuildBankTabText        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_SWAP_ITEMS,                               0x1131, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankSwapItems           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_UPDATE_TAB,                               0x13C1, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankUpdateTab           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_WITHDRAW_MONEY,                           0x020D, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankWithdrawMoney       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANKER_ACTIVATE,                               0x19E3, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildBankerActivate          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_CHANGE_NAME_REQUEST,                           0x043E, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DECLINE,                                       0x0476, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildDeclineOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DEL_RANK,                                      0x0C28, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildDelRankOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DEMOTE,                                        0x142F, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildDemoteOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DISBAND,                                       0x0E68, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildDisbandOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_EVENT_LOG_QUERY,                               0x065A, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildEventLogQueryOpcode     ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_INFO_TEXT,                                     0x0C75, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildChangeInfoTextOpcode    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_INVITE,                                        0x19A4, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildInviteOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_LEAVE,                                         0x0667, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildLeaveOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_MEMBER_SEND_SOR_REQUEST,                       0x0666, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_MOTD,                                          0x1184, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildMOTDOpcode              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_NEWS_UPDATE_STICKY,                            0x1625, STATUS_LOGGEDIN , PROCESS_INPLACE,      &WorldSession::HandleGuildNewsUpdateStickyOpcode  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_PERMISSIONS,                                   0x1436, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildPermissions             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_PROMOTE,                                       0x1467, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildPromoteOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_QUERY,                                         0x0930, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleGuildQueryOpcode             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_QUERY_NEWS,                                    0x0676, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleGuildQueryNewsOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_QUERY_RANKS,                                   0x0C37, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildQueryRanksOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_REMOVE,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildRemoveOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_REPLACE_GUILD_MASTER,                          0x163E, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_REQUEST_CHALLENGE_UPDATE,                      0x043F, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleGuildRequestChallengeUpdate  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_REQUEST_PARTY_STATE,                           0x0A8E, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildRequestPartyState       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ROSTER,                                        0x0638, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildRosterOpcode            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_ACHIEVEMENT_TRACKING,                      0x1640, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_GUILD_MASTER,                              0x0C2F, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildSetGuildMaster          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_NOTE,                                      0x1C27, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildSetNoteOpcode           ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_RANK_PERMISSIONS,                          0x0E38, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGuildSetRankPermissionsOpcode); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SWITCH_RANK,                                   0x0627, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_ADD_RECRUIT,                                0x1223, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderAddRecruit        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_BROWSE,                                     0x0BA3, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderBrowse            ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_DECLINE_RECRUIT,                            0x066D, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderDeclineRecruit    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_GET_APPLICATIONS,                           0x166E, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderGetApplications   ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_GET_RECRUITS,                               0x0430, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderGetRecruits       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_POST_REQUEST,                               0x1427, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderPostRequest       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_REMOVE_RECRUIT,                             0x0680, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderRemoveRecruit     ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_SET_GUILD_POST,                             0x0B3D, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleGuildFinderSetGuildPost      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_MEMBER_RECIPES,                          0x163D, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_RECIPES,                                 0x1438, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_REWARDS,                                 0x028D, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::HandleGuildRewardsQueryOpcode      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SET_GUILD_BANK_TEXT,                                 0x0E30, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleSetGuildBankTabText          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_SAVE_GUILD_EMBLEM,                                   0x00F6, STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSaveGuildEmblemOpcode        ); // 6.0.3 19116

    ///< Challenge & Scenario
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_SCENARIO_POI,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_ADVANCE_STEP,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_COMPLETE,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_PAUSE,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_RESTART,                                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_RESUME,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_SET_STEP,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_SHUTDOWN,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENARIO_START,                                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );   

    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_CHEAT,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_DELETE_LEADER,                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_DUMP_LEADERS,                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_LEADERS,                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_LEADERS_CHEAT,                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_MAP_STATS,                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_FINISH_CHALLENGE_MODE_CHEAT,                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GET_CHALLENGE_MODE_REWARDS,                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_RESET_CHALLENGE_MODE,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
 
    ///< Calendar
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_ADD_EVENT,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarAddEvent             );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_COMPLAIN,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarComplain             );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_COPY_EVENT,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarCopyEvent            );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_INVITE,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventInvite          );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_MODERATOR_STATUS,                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventModeratorStatus );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_REMOVE_INVITE,                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventRemoveInvite    );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_RSVP,                                 0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventRsvp            );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_SIGNUP,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventSignup          );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_STATUS,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventStatus          );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_CALENDAR,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarGetCalendar          );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_EVENT,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarGetEvent             );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_NUM_PENDING,                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarGetNumPending        );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GUILD_FILTER,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarGuildFilter          );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_REMOVE_EVENT,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarRemoveEvent          );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_UPDATE_EVENT,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarUpdateEvent          );

    ///< Battle Pay
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_GET_PRODUCT_LIST,                         0x1389, STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleBattlePayGetProductList      ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_GET_PURCHASE_LIST,                        0x1101, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_DISTRIBUTION_ASSIGN_TO_TARGET,            0x198A, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_START_PURCHASE,                           0x011E, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_CONFIRM_PURCHASE_RESPONSE,                0x012E, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_ACK_FAILED_RESPONSE,                      0x052A, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        ); // 6.0.3 19116

    ///< Garrison & Shipment
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_ALL_READY_SHIPMENTS,                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_SHIPMENT,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_SHIPMENT_CHEAT,                             0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CREATE_SHIPMENT,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_CREATE_SHIPMENT_CHEAT,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GET_SHIPMENT_INFO,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GET_SHIPMENTS_OF_TYPE,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GET_SHIPMENTS_OF_TYPE_CHEAT,                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_OPEN_SHIPMENT_NPC,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );

    DEFINE_OPCODE_HANDLER(CMSG_GARR_ADD_FOLLOWER_XP_CHEAT,                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_ADD_MISSION_CHEAT,                              0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_ADD_UNIQUE_FOLLOWER_CHEAT,                      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_ASSIGN_FOLLOWER_TO_BUILDING,                    0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_COMPLETE_MISSION,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_COMPLETE_MISSION_CHEAT,                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_CREATE_CHEAT,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_DELETE_CHEAT,                                   0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_FORCE_BUILDING_ACTIVE,                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_GENERATE_RECRUITS,                              0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_LEARN_BLUEPRINT_CHEAT,                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_LEARN_BUILDING_SPECIALIZATION_CHEAT,            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_LIST_FOLLOWERS_CHEAT,                           0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_MISSION_BONUS_ROLL,                             0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_PLACE_BUILDING_CHEAT,                           0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_PORT_CHEAT,                                     0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_PURCHASE_BUILDING,                              0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_RECRUIT_FOLLOWER,                               0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_REMOVE_BUILDING_CHEAT,                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_REMOVE_FOLLOWER_CHEAT,                          0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_REMOVE_FOLLOWER_FROM_BUILDING,                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_REQUEST_BLUEPRINT_AND_SPECIALIZATION_DATA,      0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_SET_ACTIVE_BUILDING_SPECIALIZATION,             0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_SET_ACTIVE_BUILDING_SPECIALIZATION_CHEAT,       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_SET_BUILDING_ACTIVE,                            0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_SET_FOLLOWER_ITEM_LEVEL_CHEAT,                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_SET_LEVEL_CHEAT,                                0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_START_MISSION,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_SWAP_BUILDING,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GARR_UPGRADE_BUILDING_CHEAT,                         0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_GET_GARR_INFO,                                       0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_UPGRADE_GARR,                                        0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );
    DEFINE_OPCODE_HANDLER(CMSG_SHOW_GARR_TRANSFER,                                  0x0000, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                        );

    ///< Movement
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TIME_SKIPPED,                                   0x19C2, STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleMoveTimeSkippedOpcode        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_FORCE_MOVE_ROOT_ACK,                                 0x0B73, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMoveRootAck                  ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_FORCE_MOVE_UNROOT_ACK,                               0x0B6C, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMoveUnRootAck                ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_CHARM_TELEPORT_CHEAT,                           0x1942, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_CHNG_TRANSPORT,                                 0x0969, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FALL_LAND,                                      0x0D54, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FALL_RESET,                                     0x0522, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FEATHER_FALL_ACK,                               0x1142, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleFeatherFallAck               ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK,             0x05D3, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_FLIGHT_SPEED_CHANGE_ACK,                  0x028B, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_PITCH_RATE_CHANGE_ACK,                    0x0591, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_RUN_BACK_SPEED_CHANGE_ACK,                0x0089, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK,                     0x0B7C, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_SWIM_BACK_SPEED_CHANGE_ACK,               0x0533, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_SWIM_SPEED_CHANGE_ACK,                    0x008A, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_TURN_RATE_CHANGE_ACK,                     0x0DC1, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FORCE_WALK_SPEED_CHANGE_ACK,                    0x0204, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleForceSpeedChangeAck          ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_GRAVITY_DISABLE_ACK,                            0x0553, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_GRAVITY_ENABLE_ACK,                             0x1952, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_HEARTBEAT,                                      0x19C1, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_HOVER_ACK,                                      0x0542, STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleMoveHoverAck                 ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_JUMP,                                           0x0282, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_KNOCK_BACK_ACK,                                 0x09C1, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMoveKnockBackAck             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_CAN_FLY_ACK,                                0x0202, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMoveSetCanFlyAckOpcode       ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY_ACK,    0x0284, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_COLLISION_HEIGHT_ACK,                       0x0141, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleSetCollisionHeightAck        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_FACING,                                     0x19D2, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_PITCH,                                      0x0D92, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RUN_MODE,                                   0x0D51, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_WALK_MODE,                                  0x0544, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SPLINE_DONE,                                    0x0B63, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMoveSplineDoneOpcode         ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_ASCEND,                                   0x0D41, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_BACKWARD,                                 0x0583, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_DESCEND,                                  0x0B79, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_FORWARD,                                  0x0521, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_PITCH_DOWN,                               0x0151, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_PITCH_UP,                                 0x1974, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_STRAFE_LEFT,                              0x0082, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_STRAFE_RIGHT,                             0x1151, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_SWIM,                                     0x0B7A, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_TURN_LEFT,                                0x0D14, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_TURN_RIGHT,                               0x0964, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP,                                           0x0513, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_ASCEND,                                    0x197C, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_PITCH,                                     0x0503, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_STRAFE,                                    0x01D1, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_SWIM,                                      0x097C, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_TURN,                                      0x000B, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TELEPORT_ACK,                                   0x0D01, STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMoveTeleportAck              ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TELEPORT_CHEAT,                                 0x196A, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TOGGLE_COLLISION_CHEAT,                         0x008B, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                        ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_WATER_WALK_ACK,                                 0x0543, STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMoveWaterWalkAck             ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(CMSG_WORLDPORT_RESPONSE,                                  0x0938, STATUS_TRANSFER,  PROCESS_THREADUNSAFE, &WorldSession::HandleWorldportrResponse           ); // 6.0.3 19116 

#undef DEFINE_OPCODE_HANDLER
};

void OpcodeTable::InitializeServerTable()
{
#define DEFINE_OPCODE_HANDLER(opcode, opcodeNumber, status) ValidateAndSetOpcode<(opcodeNumber < NUM_OPCODE_HANDLERS), (opcode != 0)>(opcode, opcodeNumber, #opcode, status, PROCESS_INPLACE, 0);

    DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_DATA_TIMES,                              0x0120, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_INFO_RESPONSE,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_RESTRICTED_WARNING,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ACTION_BUTTONS,                                  0x1D1F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ACTIVATETAXIREPLY,                               0x1321, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ADDON_INFO,                                      0x1D9F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ADD_RUNE_POWER,                                  0x0322, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AI_REACTION,                                     0x0BA1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AREA_SPIRIT_HEALER_TIME,                         0x1182, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_MESSAGE,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_MOVEMENT_UPDATE,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ARENA_ERROR,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ARENA_UNIT_DESTROYED,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ATTACKERSTATEUPDATE,                             0x1204, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ATTACKSTART,                                     0x1D3E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ATTACKSTOP,                                      0x1382, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ATTACK_SWING_ERROR,                              0x0509, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AURA_POINTS_DEPLETED,                            0x093B, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AURA_UPDATE,                                     0x091C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUTH_CHALLENGE,                                  0x1759, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUTH_RESPONSE,                                   0x0DA9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BARBER_SHOP_RESULT,                              0x150D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BINDER_CONFIRM,                                  0x0B11, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BINDPOINTUPDATE,                                 0x0A30, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BREAK_TARGET,                                    0x11A2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BUY_FAILED,                                      0x013A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CAMERA_SHAKE,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_AUTO_REPEAT,                              0x0B18, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CANCEL_COMBAT,                                   0x05AD, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CAST_FAILED,                                     0x1A89, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHARACTER_LOGIN_FAILED,                          0x091E, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_CREATE,                                     0x0107, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_CUSTOMIZE,                                  0x072A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_CUSTOMIZE_RESULT,                           0x0BAA, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_DELETE,                                     0x0BC4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_ENUM,                                       0x05AF, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_FACTION_CHANGE,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_RENAME,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_UNDELETE_RESPONSE,                          0x0344, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHAR_UNDELETE_COOLDOWN_RESPONSE,                 0x0310, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_BOSS_EMOTES,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWN,                                  0x0226, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWNS,                                 0x0BFA, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_TARGET,                                    0x17AE, STATUS_NEVER    );  // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CLIENTCACHE_VERSION,                             0x080D, STATUS_NEVER    );  // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CLIENT_CONTROL_UPDATE,                           0x080E, STATUS_NEVER    );  // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_COMBAT_EVENT_FAILED,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_MAP_INFO,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_PARTY_INFO,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_PLAYER_INFO,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT1,              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_SKIRMISH_QUEUE_RESULT2,              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_STATE_CHANGED,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMPLAIN_RESULT,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMPRESSED_MOVES,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMSAT_CONNECT_FAIL,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMSAT_DISCONNECT,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COMSAT_RECONNECT_TRY,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CONTACT_LIST,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CONVERT_RUNE,                                    0x1C89, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_COOLDOWN_CHEAT,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_COOLDOWN_EVENT,                                  0x05B0, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CORPSE_MAP_POSITION_QUERY_RESPONSE,              0x13AB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CORPSE_NOT_IN_INSTANCE,                          0x0D3F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CORPSE_QUERY,                                    0x09E1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CORPSE_RECLAIM_DELAY,                            0x0BE2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CREATURE_QUERY_RESPONSE,                         0x0A26, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CRITERIA_DELETED,                                0x0105, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CRITERIA_UPDATE,                                 0x1904, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CROSSED_INEBRIATION_THRESHOLD,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CUSTOM_LOAD_SCREEN,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DAMAGE_CALC_LOG,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DANCE_QUERY_RESPONSE,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DB_REPLY,                                        0x1939, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DEATH_RELEASE_LOC,                               0x098C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DEBUG_RUNE_REGEN,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DESTROY_OBJECT,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DIFFERENT_INSTANCE_FROM_PARTY,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DISENCHANT_CREDIT,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DISMOUNT,                                        0x0B89, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DISMOUNTRESULT,                                  0x000F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DISPEL_FAILED,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_GAME_ERROR,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DONT_AUTO_PUSH_SPELLS_TO_ACTION_BAR,             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DROP_NEW_CONNECTION,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_COMPLETE,                                   0x14C9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_COUNTDOWN,                                  0x020E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_INBOUNDS,                                   0x03E4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_OUTOFBOUNDS,                                0x0111, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_REQUESTED,                                  0x0827, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DUEL_WINNER,                                     0x0935, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_DUMP_RIDE_TICKETS_RESPONSE,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DURABILITY_DAMAGE_DEATH,                         0x01EC, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ECHO_PARTY_SQUELCH,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_EMOTE,                                           0x071D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ENABLE_BARBER_SHOP,                              0x13A2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ENCHANTMENTLOG,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ENVIRONMENTALDAMAGELOG,                          0x0AAB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_EQUIPMENT_SET_LIST,                              0x0F1E, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_EQUIPMENT_SET_SAVED,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_EQUIPMENT_SET_USE_RESULT,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_EXPECTED_SPAM_RECORDS,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_EXPLORATION_EXPERIENCE,                          0x07AE, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_FAILED_PLAYER_CONDITION,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_FEATURE_SYSTEM_STATUS,                           0x0B1E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_FEATURE_SYSTEM_STATUS_GLUE_SCREEN,               0x1540, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_FEIGN_DEATH_RESISTED,                            0x1B43, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_FISH_ESCAPED,                                    0x0530, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_FISH_NOT_HOOKED,                                 0x0DC0, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_FLIGHT_SPLINE_SYNC,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_FLOOD_DETECTED,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_FORCED_DEATH_UPDATE,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_FORCE_SEND_QUEUED_PACKETS,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_FORCE_SET_VEHICLE_REC_ID,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_FORGE_MASTER_SET,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_FRIEND_STATUS,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_CUSTOM_ANIM,                          0x03EB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_DESPAWN_ANIM,                         0x0D29, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_PAGETEXT,                             0x1B19, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_QUERY_RESPONSE,                       0x128A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_RESET_STATE,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GAMESPEED_SET,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GAMETIME_SET,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GAMETIME_UPDATE,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GAME_EVENT_DEBUG_LOG,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GAME_OBJECT_ACTIVATE_ANIM_KIT,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GODMODE,                                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_COMPLETE,                                 0x15D1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_MESSAGE,                                  0x1746, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_POI,                                      0x011E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GROUPACTION_THROTTLED,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_HEALTH_UPDATE,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_HIGHEST_THREAT_UPDATE,                           0x059A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_HOTFIX_INFO,                                     0x1D1D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_HOTFIX_NOTIFY,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INITIALIZE_FACTIONS,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INITIAL_SPELLS,                                  0x0A81, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_INIT_CURRENCY,                                   0x0B06, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_INIT_WORLD_STATES,                               0x0B04, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT,                                         0x052E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT_HONOR_STATS,                             0x02D3, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT_RATED_BG_STATS,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT_RESULTS_UPDATE,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INSPECT_TALENT,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_LOCK_WARNING_QUERY,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_RESET,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_RESET_FAILED,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_SAVE_CREATED,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INVALIDATE_DANCE,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INVALIDATE_PLAYER,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INVALID_PROMOTION_CODE,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_INVENTORY_CHANGE_FAILURE,                        0x0112, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LEARNED_DANCE_MOVES,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LEARNED_SPELL,                                   0x08AB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LEVELUP_INFO,                                    0x1BE4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LIST_INVENTORY,                                  0x0940, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOAD_CUF_PROFILES,                               0x1530, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOGIN_SETTIMESPEED,                              0x09AA, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOGIN_VERIFY_WORLD,                              0x0B19, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_CANCEL_ACK,                               0x0FAD, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_COMPLETE,                                 0x0B21, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_RESPONSE,                                 0x052D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOG_XPGAIN,                                      0x1B53, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MAIL_LIST_RESULT,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MAP_OBJ_EVENTS,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MEETINGSTONE_COMPLETE,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MEETINGSTONE_IN_PROGRESS,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MINIGAME_SETUP,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MINIGAME_STATE,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MINIMAP_PING,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MIRRORIMAGE_DATA,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MISSILE_CANCEL,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MODIFY_COOLDOWN,                                 0x17BA, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MONEY_NOTIFY,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOTD,                                            0x0442, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOUNTRESULT,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOUNTSPECIAL_ANIM,                               0x159F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MULTIPLE_PACKETS,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_NAME_QUERY_RESPONSE,                             0x0828, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_REALM_NAME_QUERY_RESPONSE,                       0x011D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_NEW_TAXI_PATH,                                   0x080F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_NEW_WORLD,                                       0x0206, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_NEW_WORLD_ABORT,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_NOTIFICATION,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_NOTIFY_DANCE,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_NOTIFY_DEST_LOC_SPELL_CAST,                      0x01F1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_NPC_TEXT_UPDATE,                                 0x1122, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_OFFER_PETITION_ERROR,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA,            0x053F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_CONTAINER,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_OVERRIDE_LIGHT,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PAGE_TEXT_QUERY_RESPONSE,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PARTYKILLLOG,                                    0x1312, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_COMMAND_RESULT,                            0x0B44, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_MEMBER_STATS,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PARTY_MEMBER_STATS_FULL,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PAUSE_MIRROR_TIMER,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PERIODICAURALOG,                                 0x0B1B, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PETGODMODE,                                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_ALREADY_SIGNED,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_QUERY_RESPONSE,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_RENAME_RESULT,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_SHOWLIST,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_SHOW_SIGNATURES,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PETITION_SIGN_RESULTS,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYED_TIME,                                     0x038B, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAYERBINDERROR,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYERBOUND,                                     0x078E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_DIFFICULTY_CHANGE,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_MOVE,                                     0x0ACE, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_SKINNED,                                  0x00D2, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAYER_VEHICLE_DATA,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_DANCE,                                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_MUSIC,                                      0x0981, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_OBJECT_SOUND,                               0x1C8A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_ONE_SHOT_ANIM_KIT,                          0x078D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_SOUND,                                      0x02D2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL_KIT,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PLAY_TIME_WARNING,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PONG,                                            0x17CA, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_POWER_UPDATE,                                    0x0B27, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PRE_RESURRECT,                                   0x0D2E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PROCRESIST,                                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PROPOSE_LEVEL_GRANT,                             0x1B8C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PVP_CREDIT,                                      0x13A9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PVP_LOG_DATA,                                    0x1ECA, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PVP_OPTIONS_ENABLED,                             0x0320, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PVP_SEASON,                                      0x09E3, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RAID_GROUP_ONLY,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_INSTANCE_INFO,                              0x0DA0, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RAID_INSTANCE_MESSAGE,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_MARKERS_CHANGED,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_READY_CHECK,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_READY_CHECK_COMPLETED,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_READY_CHECK_CONFIRM,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_READY_CHECK_THROTTLED_ERROR,                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_SUMMON_FAILED,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RAID_TARGET_UPDATE,                              0x0D3E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RAID_TARGET_UPDATE_ALL,                          0x000D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RANDOM_ROLL,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RANDOMIZE_CHAR_NAME,                             0x0D8F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RATED_BG_RATING,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RATED_BG_STATS,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_REALM_SPLIT,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RECEIVED_MAIL,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_REDIRECT_CLIENT,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_EXPIRED,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_FAILURE,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_REMOVED_SPELL,                                   0x0BCB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_REPORT_PVP_AFK_RESULT,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_REQUEST_CEMETERY_LIST_RESPONSE,                  0x059E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_REQUEST_PVP_REWARDS_RESPONSE,                    0x1DAE, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RESEARCH_COMPLETE,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RESEARCH_SETUP_HISTORY,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RESET_COMPRESSION_CONTEXT,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RESET_FAILED_NOTIFY,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RESURRECT_REQUEST,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RESYNC_RUNES,                                    0x00F4, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ROLE_POLL_BEGIN,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_RWHOIS,                                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_MAIL_RESULT,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SEND_UNLEARN_SPELLS,                             0x1A82, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SERVERTIME,                                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SERVER_INFO_RESPONSE,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SERVER_PERF,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_DF_FAST_LAUNCH_RESULT,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_DUNGEON_DIFFICULTY,                          0x0719, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_ATWAR,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_STANDING,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_VISIBLE,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_FLAT_SPELL_MODIFIER,                         0x1884, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_FORCED_REACTIONS,                            0x09A9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_PCT_SPELL_MODIFIER,                          0x113C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_PHASE_SHIFT,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT,                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PLAY_HOVER_ANIM,                             0x02D4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_PROFICIENCY,                                 0x00D3, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_RAID_DIFFICULTY,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_TIMEZONE_INFORMATION,                        0x153E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_PROJECTILE_POSITION,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SHOWTAXINODES,                                   0x12A1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SHOW_BANK,                                       0x1B51, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SHOW_RATINGS,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SOCKET_GEMS_RESULT,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SOR_START_EXPERIENCE_INCOMPLETE,                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLBREAKLOG,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLDAMAGESHIELD,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLDISPELLOG,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLENERGIZELOG,                                0x188A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELLHEALLOG,                                    0x0114, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELLINSTAKILLLOG,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLINTERRUPTLOG,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLLOGEXECUTE,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLLOGMISS,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELLNONMELEEDAMAGELOG,                          0x11FB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELLORDAMAGE_IMMUNE,                            0x1313, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELLSTEALLOG,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_CATEGORY_COOLDOWN,                         0x082A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_COOLDOWN,                                  0x03F4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_DELAYED,                                   0x0134, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_FAILED_OTHER,                              0x11DB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_FAILURE,                                   0x1A03, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_GO,                                        0x1161, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_START,                                     0x0803, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPELL_UPDATE_CHAIN_TARGETS,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPIRIT_HEALER_CONFIRM,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_STABLE_RESULT,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_STANDSTATE_UPDATE,                               0x1311, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_START_MIRROR_TIMER,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_START_TIMER,                                     0x0317, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_STOP_DANCE,                                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_STOP_MIRROR_TIMER,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_STREAMING_MOVIE,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SUMMON_CANCEL,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SUMMON_REQUEST,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SUPERCEDED_SPELL,                                0x0172, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SUPPRESS_NPC_GREETINGS,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SUSPEND_COMMS,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SUSPEND_TOKEN_RESPONSE,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TALENTS_ERROR,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TALENTS_INFO,                                    0x012D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TALENTS_INVOLUNTARILY_RESET,                     0x01E3, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TAXINODE_STATUS,                                 0x0338, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TEST_DROP_RATE_RESULT,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TEXT_EMOTE,                                      0x0383, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_THREAT_CLEAR,                                    0x030F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_THREAT_REMOVE,                                   0x0F3D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_THREAT_UPDATE,                                   0x03A9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TIME_ADJUSTMENT,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TIME_SYNC_REQ,                                   0x03B1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TITLE_EARNED,                                    0x1324, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TOGGLE_XP_GAIN,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TOTEM_CREATED,                                   0x0352, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TRAINER_BUY_FAILED,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TRAINER_BUY_SUCCEEDED,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TRAINER_LIST,                                    0x0BA9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TRANSFER_ABORTED,                                0x03C2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TRANSFER_PENDING,                                0x172A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TRIGGER_CINEMATIC,                               0x032F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TRIGGER_MOVIE,                                   0x151F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TURN_IN_PETITION_RESULTS,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TUTORIAL_FLAGS,                                  0x020F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_UNIT_HEALTH_FREQUENT,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_UNIT_SPELLCAST_START,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_ACCOUNT_DATA,                             0x1520, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_COMBO_POINTS,                             0x0000, STATUS_UNHANDLED); 
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_CURRENCY,                                 0x17BE, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_DUNGEON_ENCOUNTER_FOR_LOOT,               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT,                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_INSTANCE_OWNERSHIP,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_LAST_INSTANCE,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_OBJECT,                                   0x122C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_UPDATE_WORLD_STATE,                              0x03EC, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_USERLIST_ADD,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_USERLIST_REMOVE,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_USERLIST_UPDATE,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WAIT_QUEUE_FINISH,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WAIT_QUEUE_UPDATE,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WARDEN_DATA,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WARGAME_CHECK_ENTRY,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WARGAME_REQUEST_SENT,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WEATHER,                                         0x073E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_LAST_RESET,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_RESET_CURRENCY,                           0x0916, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_SPELL_USAGE,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_SPELL_USAGE_UPDATE,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WHO,                                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WHOIS,                                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_WORLD_SERVER_INFO,                               0x11AC, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_WORLD_STATE_UI_TIMER_UPDATE,                     0x0B83, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_XP_GAIN_ABORTED,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ZONE_UNDER_ATTACK,                               0x1401, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RESPEC_WIPE_CONFIRM,                             0x1341, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_TABARDVENDOR_ACTIVATE,                           0x1389, STATUS_NEVER    ); // 6.0.3 19116
    
    DEFINE_OPCODE_HANDLER(MSG_VERIFY_CONNECTIVITY,                              0x4F57, STATUS_NEVER    ); // Not an opcode


    ///< Items
    DEFINE_OPCODE_HANDLER(SMSG_SELL_ITEM,                                       0x1352, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AVERAGE_ITEM_LEVEL_INFORM,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BUY_ITEM,                                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_ADD_PASSIVE,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_COOLDOWN,                                   0x090F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_ENCHANT_TIME_UPDATE,                        0x010D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_EXPIRE_PURCHASE_REFUND,                     0x1282, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_PUSH_RESULT,                                0x0183, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_REFUND_INFO_RESPONSE,                       0x0316, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_REFUND_RESULT,                              0x0B3D, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_REMOVE_PASSIVE,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_SEND_PASSIVE,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_TEXT_QUERY_RESPONSE,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_ITEM_TIME_UPDATE,                                0x0F3E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_READ_ITEM_FAILED,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_READ_ITEM_OK,                                    0x0000, STATUS_UNHANDLED);

    ///< Achivement
    DEFINE_OPCODE_HANDLER(SMSG_ALL_ACHIEVEMENT_DATA,                            0x0030, STATUS_NEVER    );
    DEFINE_OPCODE_HANDLER(SMSG_ACHIEVEMENT_DELETED,                             0x050E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_ACHIEVEMENT_EARNED,                              0x010E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_RESPOND_INSPECT_ACHIEVEMENTS,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SERVER_FIRST_ACHIEVEMENT,                        0x1413, STATUS_UNHANDLED); // 6.0.3 19116

    ///< Trade
    DEFINE_OPCODE_HANDLER(SMSG_TRADE_STATUS,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_TRADE_STATUS_EXTENDED,                           0x0000, STATUS_UNHANDLED);

    ///< Game master
    DEFINE_OPCODE_HANDLER(SMSG_GMRESPONSE_DB_ERROR,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GMRESPONSE_RECEIVED,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GMRESPONSE_STATUS_UPDATE,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GMTICKET_CREATE,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GMTICKET_DELETETICKET,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GMTICKET_GETTICKET,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GMTICKET_SYSTEMSTATUS,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GMTICKET_UPDATETEXT,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GM_MESSAGECHAT,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GM_PLAYER_INFO,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_STATUS_UPDATE,                         0x0000, STATUS_UNHANDLED);

    ///< Void Storage
    DEFINE_OPCODE_HANDLER(SMSG_VOID_ITEM_SWAP_RESPONSE,                         0x1131, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_CONTENTS,                           0x0108, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_FAILED,                             0x0B01, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_TRANSFER_CHANGES,                   0x0321, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_VOID_TRANSFER_RESULT,                            0x0539, STATUS_NEVER    ); // 6.0.3 19116

    ///< Battlefield & Battlegroud
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_LIST,                                0x11A1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EJECTED,                         0x0DAF, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EJECT_PENDING,                   0x09A1, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_ENTERED,                         0x11A3, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE,                    0x128B, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EXIT_REQUEST,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE,          0x0B28, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_STATE_CHANGE,                    0x179A, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_PORT_DENIED,                         0x0930, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_RATED_INFO,                          0x1F0A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_QUEUED,                       0x002D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_ACTIVE,                       0x1284, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_NEEDCONFIRMATION,             0x1719, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_WAITFORGROUPS,                0x082D, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_FAILED,                       0x0308, STATUS_NEVER    ); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_INFO_THROTTLED,                     0x19E2, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_JOINED,                      0x13EA, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_LEFT,                        0x1BD1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_POSITIONS,                   0x1BE9, STATUS_UNHANDLED); // 6.0.3 19116

    ///< Auction & Black market
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_BIDDER_LIST_RESULT,                      0x01E9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_BIDDER_NOTIFICATION,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_COMMAND_RESULT,                          0x0B2D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_HELLO,                                   0x011F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_PENDING_SALES,                      0x0332, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_RESULT,                             0x0BA4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_OWNER_LIST_RESULT,                       0x0B42, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_OWNER_NOTIFICATION,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_AUCTION_REMOVED_NOTIFICATION,                    0x0000, STATUS_UNHANDLED);

    DEFINE_OPCODE_HANDLER(SMSG_BLACKMARKET_HELLO,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BLACKMARKET_REQUEST_ITEMS_RESULT,                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BLACKMARKET_BID_RESULT,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BLACKMARKET_OUT_BID,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BLACKMARKET_BID_WON,                             0x0000, STATUS_UNHANDLED);

    ///< Group
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_DECLINE,                                   0x153F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_DESTROYED,                                 0x0D2D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_INVITE,                                    0x0920, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_LIST,                                      0x00AC, STATUS_NEVER    ); // 6.0.2 18833 -- Cant find
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_SET_LEADER,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_SET_ROLE,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GROUP_UNINVITE,                                  0x0110, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_REAL_GROUP_UPDATE,                               0x0000, STATUS_UNHANDLED);

    ///< LFG
    DEFINE_OPCODE_HANDLER(SMSG_LFG_BOOT_PROPOSAL_UPDATE,                        0x07B3, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_DISABLED,                                    0x0714, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_JOIN_RESULT,                                 0x0F72, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_JOIN_RESULT,                            0x0793, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_UPDATE_BLACKLIST,                       0x0754, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_UPDATE_STATUS,                          0x0F18, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_OFFER_CONTINUE,                              0x0797, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PARTY_INFO,                                  0x0F92, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PLAYER_INFO,                                 0x0F36, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PLAYER_REWARD,                               0x0F15, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_PROPOSAL_UPDATE,                             0x0F32, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_QUEUE_STATUS,                                0x07F1, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_ROLE_CHECK_UPDATE,                           0x0736, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_ROLE_CHOSEN,                                 0x07D3, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_SLOT_INVALID,                                0x07B1, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_TELEPORT_DENIED,                             0x0795, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_SEARCH,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LFG_SEARCH_RESULTS,                              0x0798, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_STATUS,                               0x07F7, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_LFG_DUNGEON_FINDER,                         0x0F17, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_LFG_FAST_LAUNCH_RESULT,                      0x0F52, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SET_LFG_TIME_WALKER,                             0x0731, STATUS_UNHANDLED); // 6.0.3 19116

    ///< Chat & Voice Chat & Channel
    DEFINE_OPCODE_HANDLER(SMSG_AVAILABLE_VOICE_CHANNEL,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_VOICE_CHAT_STATUS,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_VOICE_PARENTAL_CONTROLS,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_VOICE_SESSION_LEAVE,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_VOICE_SESSION_ROSTER_UPDATE,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_VOICE_SET_TALKER_MUTED,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_VOICESESSION_FULL,                               0x0000, STATUS_UNHANDLED);

    DEFINE_OPCODE_HANDLER(SMSG_CHAT_IGNORED_ACCOUNT_MUTED,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_NOT_IN_PARTY,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_PLAYER_AMBIGUOUS,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_PLAYER_NOT_FOUND,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_RESTRICTED,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_SERVER_DISCONNECTED,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_SERVER_RECONNECTED,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHAT_WRONG_FACTION,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_DEFENSE_MESSAGE,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MESSAGE_BOX,                                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MESSAGECHAT,                                     0x0603, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SERVER_MESSAGE,                                  0x0683, STATUS_NEVER    ); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_LIST,                                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_MEMBER_COUNT,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_START,                                   0x016C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_UPDATE,                                  0x19DB, STATUS_NEVER    ); // 6.0.3 19116

    ///< Loot
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_AE_TARGET_ACK,                              0x031D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_AE_TARGETS,                                 0x15AF, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ALL_PASSED,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_CLEAR_MONEY,                                0x19AC, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_CONTENTS,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ITEM_NOTIFY,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_LIST,                                       0x03A4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_MASTER_LIST,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_MONEY_NOTIFY,                               0x12A9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_RELEASE_RESPONSE,                           0x0BD3, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_REMOVED,                                    0x0906, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_RESPONSE,                                   0x0F8A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLL,                                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLL_WON,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_SLOT_CHANGED,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LOOT_START_ROLL,                                 0x0000, STATUS_UNHANDLED);

    ///< Battle Pet & Pet
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_DELETED,                              0x0138, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL,                              0x19A2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL_LOCK_ACQUIRED,                0x01A3, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL_LOCK_DENINED,                 0x1BEC, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_QUERY_NAME_RESPONSE,                  0x09EC, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_SLOT_UPDATE,                          0x0000, STATUS_NEVER    );
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_UPDATE,                               0x0354, STATUS_NEVER    ); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(SMSG_PET_ACTION_FEEDBACK,                             0x0D9E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_ACTION_SOUND,                                0x0D39, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_ADDED,                                       0x0327, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_BROKEN,                                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PET_CAST_FAILED,                                 0x1914, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_DISMISS_SOUND,                               0x09A3, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_GUIDS,                                       0x150F, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_LEARNED_SPELLS,                              0x1822, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_MODE,                                        0x1D40, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_NAME_INVALID,                                0x0007, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_NAME_QUERY_RESPONSE,                         0x0D0E, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_RENAMEABLE,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_PET_SLOT_UPDATED,                                0x0B26, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_SPELLS,                                      0x0174, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_TAME_FAILURE,                                0x0025, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_UNLEARNED_SPELLS,                            0x011B, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_PET_UPDATE_COMBO_POINTS,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_PET_SPECIALIZATION,                          0x1989, STATUS_NEVER    ); // 6.0.3 19103

    ///< Quest
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_QUESTS_COMPLETED_RESPONSE,                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_QUERY_TIME_RESPONSE,                             0x1DB0, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_CONFIRM_ACCEPT,                            0x15B2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_FORCE_REMOVE,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_NPC_QUERY_RESPONSE,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_POI_QUERY_RESPONSE,                        0x1715, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_QUERY_RESPONSE,                            0x1547, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_OFFER_REWARD,                         0x17B2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_COMPLETE,                       0x1773, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_DETAILS,                        0x15B3, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_FAILED,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_INVALID,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_QUEST_LIST,                           0x17A5, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_REQUEST_ITEMS,                        0x17B8, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_STATUS,                               0x1567, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTGIVER_STATUS_MULTIPLE,                      0x17C6, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTLOG_FULL,                                   0x17E4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTUPDATE_ADD_CREDIT,                          0x1515, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTUPDATE_ADD_PVP_KILL,                        0x15A6, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTUPDATE_COMPLETE,                            0x1787, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTUPDATE_FAILED,                              0x1542, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUESTUPDATE_FAILEDTIMER,                         0x1D01, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_QUEST_PUSH_RESULT,                               0x1797, STATUS_NEVER    ); // 6.0.3 19116
    
    ///< Guild
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_DATA,                          0x1866, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_DELETED,                       0x1058, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_EARNED,                        0x1048, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_MEMBERS,                       0x1826, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_LIST,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_LOG_QUERY_RESULT,                     0x1237, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_MONEY_WITHDRAWN,                      0x1047, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_QUERY_TEXT_RESULT,                    0x1875, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_COMPLETED,                       0x1836, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_UPDATED,                         0x1A06, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHANGE_NAME_RESULT,                        0x1006, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_COMMAND_RESULT,                            0x1205, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_DATA,                             0x1208, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_DELETED,                          0x1805, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_DECLINE,                                   0x1226, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT,                                     0x0000, STATUS_UNHANDLED); // using misc packets now
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_LOG_QUERY_RESULT,                    0x1075, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_FLAGGED_FOR_RENAME,                        0x1248, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE,                                    0x1025, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE_DECLINED,                           0x1226, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_DAILY_RESET,                        0x1015, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_RECIPES,                            0x1036, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_UPDATE_NOTE,                        0x1846, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBERS_WITH_REPICE,                       0x1277, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MOVE_STARTING,                             0x1877, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_MOVED,                                     0x1838, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_NAME_CHANGED,                              0x1A07, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_NEWS_DELETED,                              0x1007, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_NEWS_UPDATE,                               0x1027, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_PARTY_STATE_RESPONSE,                      0x1225, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_PERMISSIONS_QUERY_RESULTS,                 0x1827, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_QUERY_RESPONSE,                            0x1046, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_RANK,                                      0x1218, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_RANKS_UPDATE,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_RECIPES,                                   0x1078, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_REPUTATION_REACTION_CHANGED,               0x1068, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_REPUTATION_WEEKLY_CAP,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_RESET,                                     0x1258, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_REWARDS_LIST,                              0x1818, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_ROSTER,                                    0x1026, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_UPDATE_ROSTER,                             0x1265, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_GUILD_XP_GAIN,                                   0x0B07, STATUS_UNHANDLED); // 6.0.3 19116 WTF ?
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_APPLICANT_LIST_UPDATED,                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_APPLICATIONS_LIST_CHANGED,              0x1247, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_BROWSE_UPDATED,                         0x1215, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_COMMAND_RESULT,                         0x1217, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_MEMBERSHIP_LIST_UPDATED,                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_POST_UPDATED,                           0x1817, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_LF_GUILD_RECRUIT_LIST_UPDATED,                   0x1008, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SAVE_GUILD_EMBLEM,                               0x1037, STATUS_UNHANDLED); // 6.0.3 19116

    ///< Challenge & Scenario
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_BOOT,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_OUT_OF_BOUNDS,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_POIS,                                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_PROGRESS_UPDATE,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_STATE,                                  0x0000, STATUS_UNHANDLED);

    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_ALL_MAP_STATS,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_COMPLETE,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_DELETE_LEADER_RESULT,             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_DUMP_LEADERS_RESULT,              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_MAP_STATS_UPDATE,                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD,                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_REQUEST_LEADERS_RESULT,           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_REWARDS,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CLEAR_ITEM_CHALLENGE_MODE_DATA,                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SET_ITEM_CHALLENGE_MODE_DATA,                    0x0000, STATUS_UNHANDLED);

    ///< Calendar
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_ARENA_TEAM,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_CLEAR_PENDING_ACTION,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_COMMAND_RESULT,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_ALERT,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_NOTES,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT,               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT,             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT,              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT,           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_REMOVED_ALERT,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_STATUS,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_UPDATED_ALERT,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_FILTER_GUILD,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_ADDED,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_CALENDAR,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_EVENT,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_NUM_PENDING,                       0x0000, STATUS_UNHANDLED);

    ///< Battle Pay
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_ACK_FAILED,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_CONFIRM_PURCHASE,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_DELIVERY_ENDED,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_DELIVERY_STARTED,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_DISTRIBUTION_UPDATE,                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_DISTRIBUTION_LIST_RESPONSE,       0x0F2A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE,            0x12A4, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_GET_PURCHASE_LIST_RESPONSE,           0x168A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_PURCHASE_UPDATE,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_START_DISTRIBUTION_ASSIGN_TO_TARGET_RESPONSE,    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_START_DISTRIBUTION_MAKE_PRODUCT_CHOICE_RESPONSE, 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PAY_START_PURCHASE_RESPONSE,              0x0000, STATUS_UNHANDLED);

    ///< Garrison & Shipment
    DEFINE_OPCODE_HANDLER(SMSG_COMPLETE_SHIPMENT_RESPONSE,                      0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_CREATE_SHIPMENT_RESPONSE,                        0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GET_SHIPMENT_INFO_RESPONSE,                      0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GET_SHIPMENTS_OF_TYPE_RESPONSE,                  0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_SHIPMENT_NPC_FROM_GOSSIP,                   0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_OPEN_SHIPMENT_NPC_RESULT,                        0x0000, STATUS_UNHANDLED); // 6.0.3 19116

    DEFINE_OPCODE_HANDLER(SMSG_GARR_ADD_FOLLOWER_RESULT,                        0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_ADD_MISSION_RESULT,                         0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_ASSIGN_FOLLOWER_TO_BUILDING_RESULT,         0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_BUILDING_ACTIVATED,                         0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_BUILDING_REMOVED,                           0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_BUILDING_SET_ACTIVE_SPEC_RESULT,            0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_COMPLETE_MISSION_RESULT,                    0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_DELETE_RESULT,                              0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_FOLLOWER_CHANGED_ITEM_LEVEL,                0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_FOLLOWER_CHANGED_XP,                        0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_LEARN_BLUEPRINT_RESULT,                     0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_LEARN_SPECIALIZATION_RESULT,                0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_LIST_FOLLOWERS_CHEAT_RESULT,                0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_MISSION_BONUS_ROLL_RESULT,                  0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_OPEN_ARCHITECT,                             0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_OPEN_MISSION_NPC,                           0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_PLACE_BUILDING_RESULT,                      0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_PLOT_PLACED,                                0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_PLOT_REMOVED,                               0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_REMOTE_INFO,                                0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_REMOVE_FOLLOWER_FROM_BUILDING_RESULT,       0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_REMOVE_FOLLOWER_RESULT,                     0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_REQUEST_BLUEPRINT_AND_SPEC_DATA_RESULT,     0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_START_MISSION_RESULT,                       0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GARR_UPGRADE_RESULT,                             0x0000, STATUS_UNHANDLED); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_GET_GARR_INFO_RESULT,                            0x0000, STATUS_UNHANDLED); // 6.0.3 19116

    ///< Movement
    DEFINE_OPCODE_HANDLER(SMSG_MONSTER_MOVE,                                    0x0994, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MONSTER_MOVE_TRANSPORT,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_COLLISION_DISABLE,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_COLLISION_ENABLE,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_FEATHER_FALL,                               0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_GRAVITY_DISABLE,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_GRAVITY_ENABLE,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_KNOCK_BACK,                                 0x0CCB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_LAND_WALK,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_NORMAL_FALL,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_ROOT,                                       0x1B5A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_ACTIVE_MOVER,                           0x1021, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_FLY,                                0x01F6, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY,    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_COLLISION_HEIGHT,                       0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_COMPOUND_STATE,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_BACK_SPEED,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_SPEED,                           0x100B, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_HOVER,                                  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_PITCH_RATE,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_BACK_SPEED,                         0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_SPEED,                              0x08F5, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_BACK_SPEED,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_SPEED,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_TURN_RATE,                              0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_WALK_SPEED,                             0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_TELEPORT,                                   0x03A6, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNROOT,                                     0x08BD, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_FLY,                              0x0BAE, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY,  0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_HOVER,                                0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_COLLISION_HEIGHT,                    0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_BACK_SPEED,                   0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_SPEED,                        0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_KNOCK_BACK,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_PITCH_RATE,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_BACK_SPEED,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_SPEED,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_BACK_SPEED,                     0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_SPEED,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TELEPORT,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TURN_RATE,                           0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_WALK_SPEED,                          0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_MOVE_WATER_WALK,                                 0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_DISABLE,                   0x1349, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_ENABLE,                    0x02DE, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_DISABLE,                     0x0A86, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_ENABLE,                      0x0659, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_ROOT,                                0x1959, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_ANIM,                            0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FEATHER_FALL,                    0x01FD, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED,               0x0061, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED,                    0x060C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLYING,                          0x02BD, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_HOVER,                           0x0BD6, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_LAND_WALK,                       0x01BC, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_NORMAL_FALL,                     0x11B1, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_PITCH_RATE,                      0x045C, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED,                  0x0086, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_MODE,                        0x0896, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_SPEED,                       0x1B9A, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED,                 0x0895, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_SPEED,                      0x098D, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_TURN_RATE,                       0x03F6, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_MODE,                       0x03D6, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_SPEED,                      0x0000, STATUS_UNHANDLED);
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WATER_WALK,                      0x02D5, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_START_SWIM,                          0x01BB, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_STOP_SWIM,                           0x1391, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNROOT,                              0x03B2, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_FLYING,                        0x09D9, STATUS_NEVER    ); // 6.0.3 19116
    DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_HOVER,                         0x1B4A, STATUS_NEVER    ); // 6.0.3 19116

    #undef DEFINE_OPCODE_HANDLER
};
