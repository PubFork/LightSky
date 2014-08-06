/* 
 * File:   lsSystem.h
 * Author: hammy
 *
 * Created on November 15, 2013, 9:53 PM
 */

#ifndef __LS_SYSTEM_H__
#define	__LS_SYSTEM_H__

#include <vector>

#include "lsContext.h"
#include "lsDisplay.h"
#include "lsRandom.h"
#include "lsSetup.h"

//-----------------------------------------------------------------------------
//      Forward declarations
//-----------------------------------------------------------------------------
union SDL_Event;
class lsGameState;

//-----------------------------------------------------------------------------
//      Classes
//-----------------------------------------------------------------------------
/**
 * subsystem
 * 
 * The subSystem object commands all hardware events and passes them to the
 * gameState objects that it manages. Game state objects are freed from memory
 * when this object gets destroyed, therefore, all gameState objects managed by
 * a subSystem must be created using the "new" operator.
 */
class lsSubsystem {
    private:
        /**
         * Stores the previous hardware time since the last update.
         */
        float prevTime = 0;
        
        /**
         * Duration of the last "tick," or the last time that the "run()"
         * function had been called.
         */
        float tickTime = 0.f;
        
        /**
         * A vector of game states. The game will stop running when there are
         * no more states left in the list.
         * Push a state into *this, then call "run()" in a loop in order to keep
         * a game running.
         */
        std::vector<lsGameState*> gameList;
        
        /**
         * Display object owned by *this.
         */
        lsDisplay* pDisplay = nullptr;
        
        /**
         * OpenGL render context owned by *this.
         */
        lsContext context = {};
        
        /**
         * Pseudo-Random Number Generator.
         */
        lsRandom* prng = nullptr;
        
        /**
         * Send SDL hardware events to each object in the game list.
         * 
         * @param SDL_Event&
         * A reference to the event which was generated by SDL;
         * 
         * @param lsGameState*
         * A reference to the topmost gamestate on the game list.
         */
        void passHardwareEvents(const SDL_Event&, lsGameState* const);
        
        /**
         * Update all game states in the list.
         * 
         * @param tickTime
         * The amount of time which has passed since the last update.
         */
        void updateGameStates(float tickTime);
    
    public:
        /**
         * Constructor
         */
        lsSubsystem();
        
        /**
         * Copy Constructor
         * Deleted as this would require a copy of all game states held by this
         * object.
         */
        lsSubsystem(const lsSubsystem&) = delete;
        
        /**
         * Move constructor
         * 
         * @param A subsystem object to be moved.
         */
        lsSubsystem(lsSubsystem&&);
        
        /**
         * Destructor
         * The subsystem destructor will call "terminate()," releasing the memory
         * of all gameState objects held within the gameStack.
         */
        ~lsSubsystem();
        
        /**
         * Copy operator
         * This method has been deleted in order to avoid a copy of all
         * gameStates managed by this container.
         */
        lsSubsystem& operator=(const lsSubsystem&) = delete;
        
        /**
         * Move operator
         * 
         * @param A subSystem to be moved.
         * 
         * @return A reference to *this.
         */
        lsSubsystem& operator=(lsSubsystem&&);
        
        /**
         * SubSystem initialization
         * 
         * @param lsDisplay&
         * A display object. If the display is not already running, this method
         * will cause if to be created. The display will remain attached to
         * *this object until "terminate()" is called.
         * 
         * @param useVsync
         * Determine if the display should have VSync enabled.
         * 
         * @return True if this object was successfully initialized. False if
         * something went wrong.
         */
        bool init(lsDisplay& display, bool useVsync = true);
        
        /**
         * Terminate.
         * This method will free the memory of all game states managed by this
         * container.
         */
        void terminate();
        
        /**
         * Begin a game loop.
         * This method will loop until all gameStates have either been stopped
         * (and therefore removed from this object) or until the program's main
         * display has been terminated.
         */
        void run();
        
        /**
         * Stop
         * This method will prevent the game loop from running, thereby
         * returning control back to this object's caller.
         */
        void stop();
        
        /**
         * Push a game state onto the state list. All prior states will be
         * paused, allowing the topmost state to receive hardware events.
         * 
         * @param A pointer to a gameState object allocated with "new."
         */
        bool pushGameState(lsGameState*);
        
        /**
         * Pop the last game state from the list.
         * If there are no states left in the list, the system will stop.
         */
        void popGameState();
        
        /**
         * Search for a game state in the list and remove it if it exists.
         * 
         * @param A pointer to the desired game state.
         */
        void popGameState(lsGameState*);
        
        /**
         * Search for a game state in the list and remove it if it exists.
         * 
         * @param An index of the desired game state.
         */
        void popGameState(unsigned index);
        
        /**
         * Get a game state using an index.
         * 
         * @param index
         * 
         * @return a pointer to the desired game state. Null if the index was
         * out of bounds.
         */
        lsGameState const* getGameState(unsigned index) const;
        
        /**
         * Get the index of a game state.
         * 
         * @param A pointer to the desired game state.
         * 
         * @return The index of the game state held within the game list.
         * GAME_INVALID if the state was not found.
         */
        unsigned getGameStateIndex(lsGameState*);
        
        /**
         * @return The number of states managed by this system.
         */
        unsigned getGameStackSize() const;
        
        /**
         * Get a reference to the current display object.
         * 
         * @return const lsDisplay&
         */
        const lsDisplay& getDisplay() const;
        
        /**
         * Get a reference to the current display object.
         * 
         * @return lsDisplay&
         */
        lsDisplay& getDisplay();
        
        /**
         * Get a constant reference to the object responsible for managing the
         * OpenGL render context.
         * 
         * @return const lsContext&
         */
        const lsContext& getContext() const;
        
        /**
         * Get a reference to the object responsible for managing the OpenGL
         * render context.
         * 
         * @return lsContext&
         */
        lsContext& getContext();
        
        /**
         * Get a reference to the system prng (pseudo-random number generator).
         * 
         * @return const lsRandom&
         */
        const lsRandom& getPrng() const;
        
        /**
         * Get a reference to the system prng (pseudo-random number generator).
         * 
         * @return lsRandom&
         */
        lsRandom& getPrng();
        
        /**
         * Get the current number of ticks per frame (in milliseconds).
         * 
         * @return A floating point integer that represents the number of
         * milliseconds which have passed since the last complete update.
         */
        float getTickTime() const;
        
        /**
         * Determine if *this system is still running and operational.
         * This function has the same effect as querying
         * this->getGameStackSize() > 0
         * 
         * @return bool
         * TRUE if the game list has something pushed onto it, FALSE if not.
         */
        bool isRunning() const;
};

//-----------------------------------------------------------------------------
//      Inlined Methods
//-----------------------------------------------------------------------------
/*
 * Return The number of states managed by this system.
 */
inline unsigned lsSubsystem::getGameStackSize() const {
    return gameList.size();
}

/*
 * Get a reference to the current display object.
 */
inline const lsDisplay& lsSubsystem::getDisplay() const {
    return *pDisplay;
}

/*
 * Get a reference to the current display object.
 */
inline lsDisplay& lsSubsystem::getDisplay() {
    return *pDisplay;
}

/*
 * Get a reference to the current render context.
 */
inline const lsContext& lsSubsystem::getContext() const {
    return context;
}

/*
 * Get a reference to the current render context.
 */
inline lsContext& lsSubsystem::getContext() {
    return context;
}

/*
 * Get a reference to the system prng (pseudo-random number generator).
 */
inline const lsRandom& lsSubsystem::getPrng() const {
    return *prng;
}

/*
 * Get a reference to the system prng (pseudo-random number generator).
 */
inline lsRandom& lsSubsystem::getPrng() {
    return *prng;
}

/*
 * Get the current number of ticks per frame (in milliseconds).
 */
inline float lsSubsystem::getTickTime() const {
    return tickTime;
}

/*
 * Determine if *this is still running
 */
inline bool lsSubsystem::isRunning() const {
    return gameList.size() > 0;
}

#endif	/* __LS_SYSTEM_H__ */

