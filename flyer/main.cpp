#include "common.h"
#include "queue.h"
#include "VideoProcess.hpp"
#include "serial.hpp"

sem_t sem_lo;
sem_t sem_up;
pthread_mutex_t work_mutex;

int main(int argc, const char * argv[])
{
	int fps = 25;
	int wait = 100 / fps;
	sem_init(&sem_lo, 0, 0);
	sem_init(&sem_up, 0, 0);
	pthread_mutex_init(&work_mutex, NULL);
	sem_post(&sem_lo);
	sem_post(&sem_up);

	VideoProcess *videoProcess = new VideoProcess();
	videoProcess->run();
	Serial *serial = new Serial();
	serial->run();
	while (1) {
		//imshow("upper Camera", image_up);
		//waitKey(wait/2);
		//imshow("Lower Camera", image_lo);
		//waitKey(wait/2);
	}
	return 0;
}
