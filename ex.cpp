
float clock = 0.0; /* μεταβλητή χρόνου */
int n = 0;         /* μετρητής */
int s = 500;       /* μεταβλητή κατάστασης */
int over = 0;      /* συνθήκη τερματισμού */

int main()
{
    while (!over)
    {
        clock = clock - log(random()) / 112.0; /* επόμενο γεγονός */
        U = random();
        if (U < 0.45) /* επιλογή ενέργειας */
            x = 1;
        else if (U < 0.65)
            x = 2;
        else
            x = 0;
        if (x <= s)
        {
            s = s - x;
            if (x > 0)
                n++;
        }
        else
            over = 1;
    }
}