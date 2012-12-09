#ifndef _CLOCK_H_
#define _CLOCK_H_

class Clock 
{
	public:
		Clock();
		~Clock();

		void init();
		float calculateDeltaSeconds();
		void update(float dt_real_seconds);
		void setPaused(bool is_paused);
		bool isPaused();
		
	private:
		long double total_time_;
		float last_time_;
		bool is_paused_;

};

#endif