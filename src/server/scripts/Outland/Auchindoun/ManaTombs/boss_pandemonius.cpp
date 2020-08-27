#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mana_tombs.h"

enum Texts
{
    SAY_AGGRO                       = 0,
    SAY_KILL                        = 1,
    SAY_DEATH                       = 2,
    EMOTE_DARK_SHELL                = 3
};

enum Spells
{
    SPELL_VOID_BLAST = 32325,
    SPELL_DARK_SHELL = 32358
};

enum Events
{
    EVENT_VOID_BLAST = 1,
    EVENT_DARK_SHELL
};

uint32 constexpr DARK_SHELL_EVENT_GROUP = 1;

class boss_pandemonius : public CreatureScript
{
public:
    boss_pandemonius() : CreatureScript("boss_pandemonius") { }

    struct boss_pandemoniusAI : public BossAI
    {
        boss_pandemoniusAI(Creature* creature) : BossAI(creature, DATA_PANDEMONIUS)
        {
            VoidBlastCounter = 0;
        }

        void Reset() override
        {
            _Reset();
            VoidBlastCounter = 0;
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
        }

        void KilledUnit(Unit* /*victim*/) override
        {
            Talk(SAY_KILL);
        }

        void JustEngagedWith(Unit* who) override
        {
            BossAI::JustEngagedWith(who);
            Talk(SAY_AGGRO);
            events.ScheduleEvent(EVENT_DARK_SHELL, 20s, DARK_SHELL_EVENT_GROUP);
            events.ScheduleEvent(EVENT_VOID_BLAST, 8s, 23s);
        }

        void ExecuteEvent(uint32 eventId) override
        {
            switch (eventId)
            {
                case EVENT_VOID_BLAST:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    {
                        DoCast(target, SPELL_VOID_BLAST);
                        ++VoidBlastCounter;
                    }

                    if (VoidBlastCounter == 5)
                    {
                        VoidBlastCounter = 0;
                        events.ScheduleEvent(EVENT_VOID_BLAST, 15s, 25s);
                    }
                    else
                    {
                        events.ScheduleEvent(EVENT_VOID_BLAST, 500ms);
                        events.DelayEvents(500ms, DARK_SHELL_EVENT_GROUP);
                    }
                    break;
                case EVENT_DARK_SHELL:
                    if (me->IsNonMeleeSpellCast(false))
                        me->InterruptNonMeleeSpells(true);
                    Talk(EMOTE_DARK_SHELL);
                    DoCast(me, SPELL_DARK_SHELL);
                    events.ScheduleEvent(EVENT_DARK_SHELL, 20s, DARK_SHELL_EVENT_GROUP);
                    break;
                default:
                    break;
            }
        }

        private:
            uint32 VoidBlastCounter;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetManaTombsAI<boss_pandemoniusAI>(creature);
    }
};

void AddSC_boss_pandemonius()
{
    new boss_pandemonius();
}
