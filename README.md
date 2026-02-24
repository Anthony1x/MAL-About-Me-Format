# MAL-About-Me-Format

MyAnimeList does not support Visual Novels.

I would still like to keep track of the visual novels I have read.

So I put my visual novels in the "About me" page.

However, the text in the about me treats line breaks in the file as line breaks for the actual displayed site.

This makes the About me page extremely hard to edit, because the entire table has to be kept on one singular line.

So as a workaround, I have written a script to format the file correctly for me.

Because I love feature creep, instead of manually updating the Visual Novels it now instead uses the official VNDB api
and gets the data from there. If you would like to use this for yourself, simply replace the username with yours at the
top of the `index.php` file.
