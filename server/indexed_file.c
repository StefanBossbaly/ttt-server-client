#include "indexed_file.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

#include <string.h>

#include <stdlib.h>

void index_init(indexed_file_t *file, char *master, char *index, size_t data_size)
{
	//Copy the master and index file name's our structure
	strcpy(file->master_fname, master);
	strcpy(file->index_fname, index);

	//How big is the data size
	file->data_size = data_size;
}

void index_open_transaction(indexed_file_t *file)
{
	//Open the index and master file
	file->index_fid = open(file->index_fname, O_RDWR | O_CREAT, S_IRWXU);
	file->master_fid = open(file->master_fname, O_RDWR | O_CREAT, S_IRWXU);

	//Get the amount of indexes in the file
	struct stat sb;
	fstat(file->index_fid, &sb);
	file->size = sb.st_size / sizeof(indexed_rec_t);
}

void index_close_transaction(indexed_file_t *file)
{
	//Close both files
	close(file->index_fid);
	close(file->master_fid);

	//Set the fid to -1
	file->master_fid = -1;
	file->index_fid = -1;
}

int index_get_midpoint(int min, int max)
{
	return min + ((max - min) / 2);
}

int index_get_data(indexed_file_t *file, int id, void *buffer)
{
	//Get the index of the user
	int index = index_get_index(file, id);

	//Make sure the index exists
	if (index == -1)
	{
		return -1;
	}

	//Go to the data at the index in the master file
	lseek(file->master_fid, file->data_size * index, 0);

	//Read the data from from the master file and store it into our buffer
	read(file->master_fid, buffer, file->data_size);

	return 0;
}

int index_add_index(indexed_file_t *file, int id, int index)
{
	indexed_rec_t record;
	record.id = id;
	record.index = index;

	//Insert using insertion sort
	int i;
	for (i = 0; i < file->size; i++)
	{
		indexed_rec_t current;

		//Seek to the index
		lseek(file->index_fid, i * sizeof(indexed_rec_t), SEEK_SET);

		//Read the index record from the index file
		read(file->index_fid, &current, sizeof(indexed_rec_t));

		//The current id is bigger then the id we are trying to insert, so insert it here
		if (current.id > id)
		{
			//Well here we are. This is the price we have to pay for keeping all the data in an array.
			//We have to move each index record back one so that we can insert the new record into the
			//correct location. We could have a bigger buffer like a buffer that holds multiple indexed_rec_t
			//but this will work in all cases and stresses the point.
			int j;
			for (j = file->size - 1; j >= i; j--)
			{
				//Declare a temp buffer
				indexed_rec_t buffer;

				//Move to the space
				lseek(file->index_fid, j * sizeof(indexed_rec_t), SEEK_SET);

				//Read the space
				read(file->index_fid, &buffer, sizeof(indexed_rec_t));

				//Write to [i + 1]
				write(file->index_fid, &buffer, sizeof(indexed_rec_t));
			}

			//Seek to the position in the index file where we want to insert the record
			lseek(file->index_fid, i * sizeof(indexed_rec_t), SEEK_SET);

			//Write the record to the index file
			write(file->index_fid, &record, sizeof(indexed_rec_t));

			return i;
		}
	}

	//Needs to be added at the end
	//Seek to the end of the index file
	lseek(file->index_fid, 0, 2);

	//Write to the end of the index file
	write(file->index_fid, &record, sizeof(indexed_rec_t));

	return file->size - 1;
}

int index_get_index_rec(indexed_file_t *file, int id, int min, int max)
{
	//There is no index
	if (min > max)
	{
		return -1;
	}

	//Get the midpoint
	int midpoint = index_get_midpoint(min, max);

	indexed_rec_t buffer;

	//Seek to the index
	lseek(file->index_fid, midpoint * sizeof(indexed_rec_t), 0);

	//Read the index record from the index file
	read(file->index_fid, &buffer, sizeof(indexed_rec_t));

	//See if the we have the same id
	if (id == buffer.id)
	{
		return buffer.index;
	}
	//If the id is bigger search the upper half
	else if (id > buffer.id)
	{
		return index_get_index_rec(file, id, midpoint + 1, max);
	}
	//If the id is lower search the lower half
	else
	{
		return index_get_index_rec(file, id, min, midpoint - 1);
	}
}

int index_get_index(indexed_file_t *file, int id)
{
	//Can't search a length zero array
	if (file->size == 0)
	{
		return -1;
	}

	return index_get_index_rec(file, id, 0, file->size - 1);
}

int index_add(indexed_file_t *file, int id, void *data)
{
	if (index_get_index(file, id) != -1)
	{
		return -1;
	}

	//Move the cursor to the end of the file
	lseek(file->master_fid, 0, 2);

	//Write user to the end of the file
	write(file->master_fid, data, file->data_size);

	//Update the index in our data structure
	index_add_index(file, id, file->size);

	//Increment the file size
	file->size++;

	return 0;
}

int index_update(indexed_file_t *file, int id, void *data)
{
	//Find out the index of the user
	int index = index_get_index(file, id);

	//Make sure the user exists in our index
	if (index == -1)
	{
		return -1;
	}

	//Move the cursor to the index
	lseek(file->master_fid, index * file->data_size, 0);

	//Write the user out
	write(file->master_fid, data, file->data_size);

	return 0;
}
