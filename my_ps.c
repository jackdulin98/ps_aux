#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>

void openDirectory(char* directory);
char statFile[256];

int main()
{
	openDirectory("/proc");
	return 0;
}

void openDirectory(char* inputDir)
{
	DIR* dir;
	struct dirent* currDir;

	dir = opendir(inputDir);

	// if you have a NULL directory, return an error
	if(dir == NULL)
	{
		fprintf(stderr, "The initial directory couldn't be opened.\n");
	}

	// if you don't have a NULL directory, go through all directories
	if(dir != NULL)
	{
		// before you do anything, you must print out the header
		printf("USER\t");
		printf("PID\t");
		printf("%%CPU\t");
		printf("%%MEM\t");
		printf("VSZ\t\t");
		printf("RSS\t");
		printf("TTY\t");
		printf("STAT\t");
		printf("START\t");
		printf("TIME\t");
		printf("COMMAND\n");

		do
		{
			currDir = readdir(dir);
			if(currDir == NULL)
			{
				closedir(dir);
				return;
			}
			else
			{
				// printf("The current i-node: %s\n", currDir->d_name);
				if(currDir->d_type == DT_DIR)
				{

                                        // starttime variables
					strcat(statFile, inputDir);
					strcat(statFile, "/");
					strcat(statFile, currDir->d_name);
					strcat(statFile, "/");
					strcat(statFile, "status");

					// write out the contents of the file
					int fd = open(statFile, O_RDONLY);
					if(fd >= 0)
					{
						char buffer[1500];
						int idx = 0;
						int numnewlines = 0;

						// the uid fields
						char uidnum[7];
						int uididx = 0;

						int sizeRead = read(fd, buffer, 1500);
						// printf("The size of the file read was = %d\n", sizeRead);

						while(*(char*)(buffer + idx) != '\0')
						{
							if(numnewlines == 8)
							{
								while(*(char*)(buffer + idx) != '\t')
								{
									++idx;
								}
								++idx;
								while(*(char*)(buffer + idx) != '\t')
								{
									uidnum[uididx] = *(char*)(buffer + idx);
									++uididx;
									++idx;
									if(*(char*)(buffer + idx) == '\t')
									{
										uidnum[uididx] = '\0';
									}
								}
								int uid_num = atoi(uidnum);
								struct passwd* username = getpwuid(uid_num);
								printf("%s\t", username->pw_name);
								close(fd);
								break;
							}

							if(*(char*)(buffer + idx) == '\n')
							{
								++numnewlines;
							}
							++idx;
						}

						// now, give the process ID
						printf("%s\t", currDir->d_name);

						// calculate the CPU usage

						// first, go into the /proc/uptime folder
						bzero(statFile, 256);

						strcat(statFile, inputDir);
						strcat(statFile, "/");
						strcat(statFile, "uptime");

						fd = open(statFile, O_RDONLY);
						double timeup;
						if(fd >= 0)
						{
							int idx = 0;
							char uptime[50];
							int sizeRead = read(fd, uptime, 50);
							// just want to read the first number in uptime
							while(*(char*)(uptime + idx) != ' ')
							{
								uptime[idx] = *(char*)(uptime + idx);
								++idx;
							}
							uptime[idx + 1] = '\0';
							timeup = atof(uptime);
							close(fd);
						}

						// then, go to the /proc/[uid]/stat folder
						bzero(statFile, 256);

						strcat(statFile, inputDir);
						strcat(statFile, "/");
						strcat(statFile, currDir->d_name);
						strcat(statFile, "/");
						strcat(statFile, "stat");

						fd = open(statFile, O_RDONLY);

						// so you can use the majorcode outside of that scope
						char majorcode[2];

						// so you can use session id outside of that scope
						int session_id;

						// so you can use the niceness value outside of that scope
						long int niceness_value;

						// so you can see the amount of threads outside of that scope
						int num_threads;

						// so you can see the foreground process id outside of that scope
						int fpi_num;

						// so you can use vmem_size outside of that scope
						unsigned long vmem_size;

						// so you can use the time values outside of that scope
						long hertz_value;
						double proc_time;
						double elapsed_time;

						if(fd >= 0)
						{
							char buf[1000];
							int idx = 0;
							int sizeRead = read(fd, buf, 1000);

							// major code
							int majorcodeidx = 0;

							// session id variables
							char sessionid[20];
							int sessionidx = 0;

							// foreground process id variables
							char fpi[2000];
							int fpiidx;

							// utime variables
							char utime[20];
							int utimeidx = 0;
							long int u_time;

							// stime variables
							char stime[20];
							int stimeidx = 0;
							long int s_time;

							// niceness variables
							char niceness[3];
							int nicenessidx = 0;

							// thread variables
							char threads[10];
							int threadsidx = 0;

							// starttime variables
							char starttime[20];
							int starttimeidx = 0;
 							long int start_time;

							// vsz variables
							char vsz[30];
							int vszidx = 0;

							// keep track of the number of spaces
							int numspaces = 0;

							// go through the /proc/[pid]/stat file
							while(*(char*)(buf + idx) != '\0')
							{
								// state used to determine the major code
								if(numspaces == 2)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										majorcode[majorcodeidx] = *(char*)(buf + idx);
										++majorcodeidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											majorcode[majorcodeidx] = '\0';
											break;
										}
									}
								}

								// to get the session id
								if(numspaces == 5)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										sessionid[sessionidx] = *(char*)(buf + idx);
										++sessionidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											sessionid[sessionidx] = '\0';
											break;
										}
									}
									session_id = atoi(sessionid);
								}

								// to get the utime
								if(numspaces == 13)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										utime[utimeidx] = *(char*)(buf + idx);
										++utimeidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											utime[utimeidx] = '\0';
											break;
										}
									}
									u_time = atoi(utime);
								}

								// to get the stime
								if(numspaces == 14)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										stime[stimeidx] = *(char*)(buf + idx);
										++stimeidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											stime[stimeidx] = '\0';
											break;
										}
									}
									s_time = atoi(stime);
								}

								// to determine the niceness of a process
								if(numspaces == 18)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										niceness[nicenessidx] = *(char*)(buf + idx);
										++nicenessidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											niceness[nicenessidx] = '\0';
											break;
										}
									}
									niceness_value = atol(niceness);
								}

								// to get the number of threads
								if(numspaces == 19)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										threads[threadsidx] = *(char*)(buf + idx);
										++threadsidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											threads[threadsidx] = '\0';
											break;
										}
									}
									num_threads = atoi(threads);
								}

								// to get the starttime
								if(numspaces == 21)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										starttime[starttimeidx] = *(char*)(buf + idx);
										++starttimeidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											starttime[starttimeidx] = '\0';
											break;
										}
									}
									start_time = atoi(starttime);

									// take all of the info, combine it to get %CPU
									hertz_value = sysconf(_SC_CLK_TCK);
									proc_time = u_time + s_time;	// don't count child processes
									elapsed_time = timeup - (start_time / hertz_value);	// duration of the process
									double cpu_pct = 100 * ((proc_time / hertz_value) / elapsed_time);
									printf("%3.1f\t", cpu_pct);
								}

								// to get the vms field
								if(numspaces == 22)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										vsz[vszidx] = *(char*)(buf + idx);
										++vszidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											vsz[vszidx] = '\0';
											break;
										}
									}
									vmem_size = atol(vsz);
									close(fd);
								}

								if(*(char*)(buf + idx) == ' ')
								{
									++numspaces;
								}
								++idx;
							}
						}

						bzero(statFile, 256);

						// now, the %MEM attribute will be filled in

						// for this, read the /proc/[pid]/statm
						strcat(statFile, inputDir);
						strcat(statFile, "/");
						strcat(statFile, currDir->d_name);
						strcat(statFile, "/");
						strcat(statFile, "statm");

						long int rss_num;		// the scope was expanded for future access

						fd = open(statFile, O_RDONLY);
						if(fd >= 0)
						{
							char pen[50];
							char rssnum[20];
							int rssnumidx = 0;
							int idx = 0;
							int numspaces = 0;
							int sizeRead = read(fd, pen, 50);
							while(*(char*)(pen + idx) != '\0')
							{
								if(numspaces == 1)
								{
									while(*(char*)(pen + idx) != ' ')
									{
										rssnum[rssnumidx] = *(char*)(pen + idx);
										++rssnumidx;
										++idx;
										if(*(char*)(pen + idx) == ' ')
										{
											rssnum[rssnumidx] = '\0';
										}
									}
									rss_num = atof(rssnum);
								}
								if(*(char*)(pen + idx) == ' ')
								{
									++numspaces;
								}
								++idx;
							}
							close(fd);
						}

						// now, I want to open /proc/meminfo
						bzero(statFile, 256);
						strcat(statFile, inputDir);
						strcat(statFile, "/");
						strcat(statFile, "meminfo");

						fd = open(statFile, O_RDONLY);
						if(fd >= 0)
						{
							char pencil[80];
							char totalmem[20];
							int totalmemidx = 0;
							long int total_mem;
							int idx = 0;
							int numspaces = 0;
							int sizeRead = read(fd, pencil, 80);

							// only need to read to the new line
							while(*(char*)(pencil + idx) != '\n')
							{
								// 7 spaces before each field
								if(numspaces == 7)
								{
									while(*(char*)(pencil + idx) != ' ')
									{
										totalmem[totalmemidx] = *(char*)(pencil + idx);
										++totalmemidx;
										++idx;
										if(*(char*)(pencil + idx) == ' ')
										{
											totalmem[totalmemidx] = '\0';
										}
									}
									total_mem = atoi(totalmem);
									double mem_pct = (double)(rss_num/total_mem);
									printf("%3.1f\t", mem_pct);
									close(fd);
								}
								if(*(char*)(pencil + idx) == ' ')
								{
									++numspaces;
								}
								++idx;
							}
						}

						// now, we want the VSZ attribute
						vmem_size /= 1000;		// must convert to KB from bytes
						printf("%lu\t\t", vmem_size);

						// now, we want the RSS attribute
						printf("%ld\t", rss_num);

						// the TTY attribute is just a question mark
						printf("?\t");

						// to get the STAT parameter
						printf("%s", majorcode);

						// could add more minor codes onto it:

						// to get the niceness value - extra possible minor code
						if(niceness_value != 0)
						{
							if(niceness_value < 0)
							{
								printf("<");
							}
							else
							{
								printf("N");
							}
						}

						// to get the VmLock attribute, look in /proc/[pid]/status
						bzero(statFile, 256);
						strcat(statFile, inputDir);
						strcat(statFile, "/");
						strcat(statFile, currDir->d_name);
						strcat(statFile, "/");
						strcat(statFile, "status");

						fd = open(statFile, O_RDONLY);
						if(fd >= 0)
						{
							char buffer[1500];
							int idx = 0;
							int numnewlines = 0;

							// the vmrss fields
							char vmrss[10];
							int vmidx = 0;

							// check to see if it's a vmrss field
							char line[100];
							int lineidx = 0;

							int sizeRead = read(fd, buffer, 1500);

							while(*(char*)(buffer + idx) != '\0')
							{
								if(numnewlines == 14)
								{
									while(*(char*)(buffer + idx) != '\n')
									{
										line[lineidx] = *(char*)(buffer + idx);
										++idx;
										++lineidx;
										if(*(char*)(buffer + idx) == '\n')
										{
											line[lineidx] = '\0';
											break;
										}
									}
									char* token = strtok(line, "\t");

									// check for the VmLock attribute - is it there or not?
									if(strcmp(token, "VmLck:") == 0)
									{
										token = strtok(NULL, " ");
										int check = atoi(token);

										// if the VmLock attribute is present and isn't zero, there are pages locked in memory
										if(check != 0)
										{
											printf("L");
										}
										close(fd);
									}
									else
									{
										close(fd);
									}
								}
								if(*(char*)(buffer + idx) == '\n')
								{
									++numnewlines;
								}
								++idx;
							}
						}

						// if the session_id == pid, the process is a session leader
						int real_pid = atoi(currDir->d_name);
						if(real_pid == session_id)
						{
							printf("s");
						}

						// if a given process has more than one thread, it is multi-threaded
						if(num_threads > 1)
						{
							printf("l\t");
						}
						else
						{
							printf("\t");
						}

						// to get the foreground process ID, look in /proc/[pid]/stat
						long int fore_id;

						bzero(statFile, 256);
						strcpy(statFile, inputDir);
						strcpy(statFile, "/");
						strcpy(statFile, currDir->d_name);
						strcpy(statFile, "/");
						strcpy(statFile, "stat");

						fd = open(statFile, O_RDONLY);
						if(fd >= 0)
						{
							char buf[1000];
							char fore[20];
							int foreidx = 0;
							int idx = 0;
							int sizeRead = read(fd, buf, 1000);
							int numspaces = 0;

							while(*(char*)(buf + idx) != '\0')
							{
								if(numspaces == 7)
								{
									while(*(char*)(buf + idx) != ' ')
									{
										fore[foreidx] = *(char*)(buf + idx);
										++foreidx;
										++idx;
										if(*(char*)(buf + idx) == ' ')
										{
											fore[foreidx] = '\0';
											fore_id = atol(fore);
											if(fore_id == real_pid)
											{
												printf("+\t");
											}
											else
											{
												printf("\t");
											}
											close(fd);
											break;
										}
									}
								}
							}
						}

						// want the START attribute
						// re-use the elapsed_time variable (time since the process started)

						time_t currTime = time(NULL);
						long int backTime = currTime - elapsed_time;
						struct tm *backDate = localtime(&backTime);

						// if the process was created on the same day, elapsed_time < 86400 (seconds in a day)
						if(elapsed_time < 86400)
						{
							printf("%d:", backDate->tm_hour);
							if(backDate->tm_min < 10)
							{
								printf("0");
							}
							printf("%d\t", backDate->tm_min);
						}
						else
						{
							printf("%d ", backDate->tm_mday);
							switch(backDate->tm_mon)
							{
								case 0:
									printf("JAN\t");
									break;
								case 1:
									printf("FEB\t");
									break;
								case 2:
									printf("MAR\t");
									break;
								case 3:
									printf("APR\t");
									break;
								case 4:
									printf("MAY\t");
									break;
								case 5:
									printf("JUN\t");
									break;
								case 6:
									printf("JUL\t");
									break;
								case 7:
									printf("AUG\t");
									break;
								case 8:
									printf("SEP\t");
									break;
								case 9:
									printf("OCT\t");
									break;
								case 10:
									printf("NOV\t");
									break;
								case 11:
									printf("DEC\t");
									break;
							}
						}
						// want the TIME attribute - to get this, use proc_time / hertz_value
						long int TIME = (long int)(proc_time / hertz_value);

						// to get TIME in minutes and seconds
						printf("%ld:", TIME / 60);
						if((TIME % 60) < 10)
						{
							printf("0");
						}
						printf("%ld\t", TIME % 60);

						// now, we want the COMMAND attribute
						bzero(statFile, 256);

						// to get that attribute, go into the /proc/[pid]/cmdline folder
						strcat(statFile, inputDir);
						strcat(statFile, "/");
						strcat(statFile, currDir->d_name);
						strcat(statFile, "/");
						strcat(statFile, "cmdline");

						fd = open(statFile, O_RDONLY);
						if(fd >= 0)
						{
							char* pens = (char*)malloc(8000);
							int idx = 0;

							int sizeRead = read(fd, pens, 8000);

							// take the contents of that file and print it out
							char* token = strtok(pens, "\0");
							if(token != NULL)
							{
								printf("%s\n", token);
							}
							else
							{
								printf("\n");
							}

							free(pens);
							close(fd);
						}
					}
					bzero(statFile, 256);
				}
			}
		}while(currDir != NULL);
	}
}

