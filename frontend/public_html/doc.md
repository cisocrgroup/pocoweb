# PoCoWeb documentation

PoCoWeb is a web based post-correction system for OCRed historical
documents.  It is based on
[PoCoTo](https://github.com/cisocrgroup/PoCoTo).  PoCoWeb consists of
a backend that offers a REST API for the post-correction and a
frontend that facilitates the post-correction of historical documents
using a web-browser.

Users can upload their OCRed documents and correct them in the web
front end.  The corrected documents can then be downloaded and further
processed.  PoCoWeb understands various OCR formats that can be used.
Any corrections are written back into the original format of the
documents.  This makes it possible to integrate the manual
post-correction of OCRed documents into other digitalisation
work flows.

As PoCoTo, PoCoWeb is connected with a language
[profiler](https://github.com/cisocrgroup/Profiler) that enables
semi-automatic correction of historical documents.  The profiler is
able to differentiate between historical spelling variation and real
OCR errors in the documents and generates correction suggestions for
suspicious words.

<img width="50%" src="assets/images/doc/welcome.png" alt="Main page"/>

PoCoWeb offers:
* Simple user management to enable parallel correction of documents
  packages.
* The possibility to split documents into packages to parallel the
  manual post-correction.
* A connection to the language profiler that:
  * generates correction suggestions.
  * lists suspicious words and error patterns.
  * adapts to the manual correction on the document.
* An interface to an automatic correction system that:
  * uses the language profiler.
  * automatically generates lexicon extensions for the profiler.
  * corrects the document
  * allows for interactive manual inspection of the correction.
* The possibility to download a global pool of corrected lines in the
  whole corpus to be used for OCR-training etc.
* An extensive REST API to enable the automation of different aspects
  of the post-correction.

- - -
## Table of contents
* [Login](#user-content-login)
* [Users](#user-content-users)
  * [Account settings](#user-content-account-settings)
  * [User management](#user-content-user-management)
* [Projects](#user-content-projects)
  * [Creating new projects](#user-content-upload-new-project)
  * [Project archives](#user-content-project-archives)
  * [Project overview](#user-content-project-table)
  * [Deleting of projects and packages](#user-content-delete-projects)
  * [Splitting and reclaiming of projects and packages](#user-content-split-projects)
  * [Downloading projects](#user-content-download-project)
  * [Profiling projects](#user-content-profile-project)
* [Post correction](#user-content-post-correction)
  * [Page correction](#user-content-page-correction)
  * [Navigation bar](#user-content-navigation-bar)
  * [Concordance view](#user-content-concordance-view)
* [Installation](#user-content-installation)
* [Grond-truth pool](#user-content-pool)
* [Services](#user-content-overview-services)
* [REST API](#user-content-rest-api)
  * [Authentification](#user-content-authorization)
  * [GET rest/api-version](#user-content-api-get-version)
  * [POST rest/login](#user-content-api-post-login)
  * [GET rest/login](#user-content-api-get-login)
  * [POST rest/users](#user-content-api-post-users)
  * [GET rest/users](#user-content-api-get-users)
  * [PUT rest/users/`uid`](#user-content-api-post-users-uid)
  * [GET rest/users/`uid`](#user-content-api-get-users-uid)
  * [DELETE rest/users/`uid`](#user-content-api-delete-users-uid)
  * [GET rest/books](#user-content-api-get-books)
  * [POST rest/books](#user-content-api-post-books)
  * [GET rest/books/`pid`](#user-content-api-get-books-pid)
  * [PUT rest/books/`pid`](#user-content-api-put-books-pid)
  * [DELETE rest/books/`pid`](#user-content-api-delete-books-pid)
  * [GET rest/books/`pid`/statistics](#user-content-api-get-books-pid-statistics)
  * [POST rest/pkg/split/books/`pid`](#user-content-api-post-books-pid-split)
  * [GET rest/pkg/assign/books/`pid`](#api-get-books-pid-assign)
  * [GET rest/pkg/takeback/books/`pid`](#api-get-books-pid-takeback)
  * [GET rest/books/`pid`/download](#user-content-api-get-books-pid-download)
  * [GET rest/books/`pid`/search](#user-content-api-get-books-pid-search)
  * [GET rest/books/`pid`/pages/`pageid`](#user-content-api-get-books-pid-pages-pageid)
  * [DELETE rest/books/`pid`/pages/`pageid`](#user-content-api-delete-books-pid-pages-pageid)
  * [GET rest/books/`pid`/pages/first](#user-content-api-get-books-pid-pages-first)
  * [GET rest/books/`pid`/pages/last](#user-content-api-get-books-pid-pages-last)
  * [GET rest/books/`pid`/pages/`pageid`/next/`n`](#user-content-api-get-books-pid-pages-pageid-next-n)
  * [GET rest/books/`pid`/pages/`pageid`/prev/`n`](#user-content-api-get-books-pid-pages-pageid-prev-n)
  * [GET rest/books/`pid`/pages/`pageid`/lines/`lid`](#user-content-api-get-books-pid-pages-pageid-lines-lid)
  * [POST/PUT rest/books/`pid`/pages/`pageid`/lines/`lid`](#user-content-api-post-books-pid-pages-pageid-lines-lid)
  * [POST/PUT rest/books/`pid`/pages/`pageid`/lines/`lid`?t=ocr](#user-content-api-post-books-pid-pages-pageid-lines-lid-ocr)
  * [DELETE rest/books/`pid`/pages/`pageid`/lines/`lid`](#user-content-api-delete-books-pid-pages-pageid-lines-lid)
  * [POST rest/books/`pid`/pages/`pageid`/lines/`lid`/tokens/`tid`](#user-content-api-post-books-pid-pages-pageid-lines-lid-tokens-tid)
  * [PUT rest/books/`pid`/pages/`pageid`/lines/`lid`/tokens/`tid`](#user-content-api-post-books-pid-pages-pageid-lines-lid-tokens-tid)
  * [GET rest/books/`pid`/pages/`pageid`/lines/`lid`/tokens/`tid`](#user-content-api-get-books-pid-pages-pageid-lines-lid-tokens-tid)
  * [GET rest/profile/books/`pid`](#user-content-api-get-books-pid-profile)
  * [POST rest/profile/books/`pid`](#user-content-api-post-books-pid-profile)
  * [POST rest/profile/suspicious/books/`pid`](#user-content-api-get-books-pid-suspicious)
  * [GET rest/profile/languages](#user-content-api-get-profiler-languages)
  * [GET rest/profile/adaptive/books/`pid`](#user-content-api-post-books-pid-adaptive-tokens)
  * [GET rest/postcorrect/le/books/`pid`](#user-content-api-get-el)
  * [PUT rest/postcorrect/le/books/`pid`](#user-content-api-put-el)
  * [POST rest/postcorrect/le/books/`pid`](#user-content-api-post-el)
  * [GET rest/postcorrect/books/`pid`](#user-content-api-get-rrdm)
  * [POST rest/postcorrect/books/`pid`](#user-content-api-post-rrdm)
  * [POST rest/postcorrect/train/books/`pid`](#user-content-api-post-retrain)
  * [GET rest/pool/global](#user-content-api-get-global-pool)
  * [GET rest/pool/user](#user-content-api-get-user-pool)
  * [GET rest/jobs/`jobid`](#user-content-api-get-jobs)

- - -
<a id='login'></a>
## Login
In order to use PoCoWeb, each user has to login with an appropriate
user account using the login link at the right top of the page.  Users
can logout again using the logout link at the same position.

<img width="50%" src="assets/images/doc/login.png" alt="PoCoWeb login"/>

- - -
<a id='users'></a>
## Users

In general there are two kinds of user accounts in PoCoWeb.  The two
types have differen permissings in the system:

1. Administrator accounts
   * Create new administrator and normal users accounts
   * Delete user accounts
   * Upload new documents and create new
     [projects](#user-content-projects)
   * Split projects into [packages](#user-content-projects)
   * [Assign](#user-content-split-projects) packages to different user
     accounts
   * [Profile](#user-content-profile-project) projects.
   * [Post correct](#user-content-post-correction) projects and
     packages.
   * [Delete](#user-content-delete-projects) documents and packages.
   * [Download](#user-content-download-project) corrected projects.
   * Download a [pool](#user-content-pool) of corrected projects.
2. Normal user accounts
   * Post correct [packages](#user-content-projects) that
     where assigned to them
   * [Profile](#user-content-profile-project) projects.
   * [Reassign](#user-content-split-projects) packages back to their
     original owner.

- - -
<a id='account-settings'></a>
### Account settings

Users can manage their account settings using the Account tab on the page.
Each user can only manage their own user's settings.  It is not possible to change
the settings of another user account even if you are logged in with an
administrator account.

<img width="50%" src="assets/images/doc/user_account.png" alt="Account settings"/>

In order to update user settings or to change the user's password,
fill in or change the appropriate fields and click on the ![update
account
settings](assets/images/doc/button-update-account-settings.png)
button.

If you want to delete your account click on the ![delete
account](assets/images/doc/button-delete-account.png) button.  This
will delete the user and all its projects and packages.  Use with
caution.

Note: Using the account settings is the only way to delete an
administrator account (see [User
management](#user-content-user-management) below).  So in order to
delete an administrator account one has to login with the account and
delete it using the account settings page.

- - -
<a id='user-management'></a>
### User management

User management is done via the User tab.  You have to be logged in
with an administrator account in order to access and use the user
management page.

<img width="50%" src="assets/images/doc/users_list.png" alt="User management"/>

The user management page consist of an overview list of all users in
the system.  In order to remove a user account (the user cannot be an
administrator), click on the
![remove](assets/images/doc/glyphicon-remove.png) button in the
according user entry in the user table.

To delete an administrator account log in with this account, go to the
[Account tab](#user-content-account-settings) and click to the ![delete
account](assets/images/doc/button-delete-account.png) button.

To create new user accounts Click to the ![create
user](assets/images/doc/button-create-new-user.png) button and fill
out the input fields accordingly.  If the account should be an
administrator account, do not forget to check the ![administrator
check box](assets/images/doc/checkbox-admin.png) check box.

<img width="50%" src="assets/images/doc/user_new_modal.png" alt="Create a user"/>

- - -
<a id='projects'></a>
## Projects

Project management is done via the project tile or the project tab.
This page lists all available projects and packages (see below) that
your user owns.  On this page you can upload new project archives (see
below). In order to process a project or package simply select the
appropriate entry in the project table.

<img width="50%" src="assets/images/doc/project_list.png" alt="Project list"/>

In the list you can see some basic information about the available
projects and packages.  Projects and packages are marked with either
the project symbol ![project
symbol](assets/images/doc/project_image.png) or the package symbol
![package symbol](assets/images/doc/package_image.png).

From this page you can also [create new
projects](#user-content-upload-new-project) and download a
[pool](#user-content-pool) of all you user's projects or all globally
pooled projects.

PoCoWeb uses two different kinds of correctable documents.  *Projects*
on the one hand represent whole documents or books that should be
corrected.  *Packages* on the other hand are smaller subsets of
projects that contain a subset of the pages of their parent project.
Two packages with the same parent never contain overlapping pages.
This makes it possible for two different users to correct different
packages of the same project in parallel.

Only administrators can upload new projects, split them into a number
of packages and assign those packages to different users.  It is not
possible to assign an existing project to a different user.

<div class='doc-note'> Note: Since it is possible to correct a whole
project as well, one should never correct a project while other users
could be correcting an associated package.  This could lead to cases
where one user accidentally overrides an other users work. </div>

<div class='doc-note'> Note: For historical reasons the api uses the
term «books» to refer to projects and the term «projects» to refer to
packages.  So for example in the REST API `projectId` refers to the id
of a package and `bookId` refers to the id of a project.  Hopefully
this issue can be solved soon. </div>

<a id='upload-new-project'></a>
### Creating new projects

If you are logged in with an administrator account, you can create a
new project.  Fill in the meta data fields in the mask, select the
according [project archive](#user-content-project-archives) on your
computer and click on the
![upload](assets/images/doc/button-upload.png) button.  Depending on
the size of the project archive, the uploading of the project can take
a while.

Do not forget to set the right language for the new project if you are
planning to use the profiler.  The language field lists all available
languages for the chosen profiler.

<img width="50%" src="assets/images/doc/create_new_project.png" alt="Project list"/>

The [PoCoWeb back-end](#user-content-PoCoWeb-backend) analyzes the
project archive, processes its OCR and image files and publishes the
project.  If the uploading was successfully, you should see the new
project in the project table.

The project's author, title and year can be filled out as it seems
fit.  Theses values are not used internally and are there for an easy
reference.

A project's profiler and its language are settings specific for the
language profiler.  If you plan to use a profiler for the post
correction of your project, you have to set them accordingly.  The
normal installation of PoCoWeb comes with a local configured profiler,
you can use `default` or `local` as value for your profiler.  If not
or if you want to use another profiler, you have to specify its URL.

<a id='project-archives'></a>
### Project archives

A project archive is a zipped directory structure that contains the
OCR and image files for the pages of a document.  Image files should
be encoded as PNG files if possible, but specify or TIFF encoded
images are fine, too.  The OCR files should be either [ABBYY-XML](),
[ALTO-XML](), [PAGE-XML]() or [hOCR]() encoded files.

The back-end tries its best to automatically search and order the
pages in project archives.  Therefore the matching OCR and image files
should have the same name (without the extension).  It does not matter
in which directory structure the different image and OCR files reside.
If the matching OCR and image files do have the same filenames for any
reason, you can add a [METS/MOTS]() meta data file to the archive, in
which the page ordering and the association between image and OCR
files is specified.

Example structure of a project archive:

```
archive
├── img
│   ├── page-0001.png
│   ├── page-0002.png
│   └── page-0003.png
└── ocr
    ├── page-0001.xml
    ├── page-0002.xml
    └── page-0003.xml
```

There is no need to separate the image and OCR files with different directories.
You can also use a flat directory structure:
```
archive
├── page-0001.png
├── page-0001.xml
├── page-0002.png
├── page-0002.xml
├── page-0003.png
└── page-0003.xml
```

It is also possible to have the files in a page directory structure like this:
```
archive
├── page-0001
│   ├── page.xml
│   └── page.png
├── page-0003
│   ├── page.xml
│   └── page.png
└── page-0002
    ├── page.xml
    └── page.png
```

If you use [ocropy](https://github.com/tmbdev/ocropy),
you can also use its project structure directly to upload a project archive:
```
archive
└── book
    ├── 0001.png
    ├── 0001
    │   ├── 010001.bin.png
    │   ├── 010001.txt
    │   └─ ...
    ├── 0002.png
    ├── 0002
    │   ├── 010001.bin.png
    │   ├── 010001.txt
    │   └─ ...
    ├── 0003.png
    └── 0003
        ├── 010001.bin.png
        ├── 010001.txt
        └─ ...
```

<a id='project-table'></a>
### Project overview

If you select a project or package the system opens the project
overview.  Depending if you are handling a project or package and if
you are logged as normal user or as administrator you will see a
different page with different edit options.

<img width="50%" src="assets/images/doc/project_overview.png" alt="Project overview"/>

On the top the system presents you some basic information about the
project.  Below you see the correction status of the project, listing
the percentage of corrected lines and tokens and the (estimated) word
error rate of the project.

<div class='doc-note'> Note: The word error rate is based only on the
manually corrected tokens.  As long as not all tokens are manually
corrected, the number is only a rough estimate.</div>

Below is a list of tiles that offer access to different edit options
(the actual list may be missing some of the tiles):
1. Click on the __Order Profile__ tile to
   [profile](#user-content-profile-project) the project.
2. Click on the __Automatic Postcorrection__ tile to start the
   automatic post-correction.
3. Click on the __Manual Postcorrection__ tile to start to [manually
   correct](#user-content-post-correction) the document.
4. Click on the __Adaptive tokens__ tile to get a list of the
   adaptive tokens (manually corrected tokens) in the project.
5. Click on the __Split__ tile to
   [split](#user-content-split-projects) a project into packages.
6. Click on the __Edit__ tile to edit the meta information about
   the project.
7. Click on the __Download__ tile to
   [download](#user-content-download-project) the project.
8. Click on the __Delete__ tile to
   [delete](#user-content-delete-projects) this project.
9. Click on the __Reclaim package__ tile to
   [reclaim](#user-content-split-projects) all packages for the
   project.

<a id='delete-projects'></a>
### Deleting projects and packages
Both projects and packages can be deleted.  If you delete a package
only the according package is deleted.  No content of the parent
project is touched.  On the other hand, if you delete a project all
content is deleted and is irretrievable lost.

You cannot no delete a project that contains packages.  So in order to
delete a project one has to [reclaim](#user-content-split-projects)
all packages and delete them beforehand.

<a id='split-projects'></a>
### Splitting and reclaiming of projects and packages

You can split a project into different packages and assign each
package to a different user (both normal and administrators).
Packages are generated by distributing the pages of the project as
evenly as possible among the packages.  You can select to randomly
distribute the pages instead of sequentially assigning them.

<img width="50%" src="assets/images/doc/split_project_modal.png" alt="Split project"/>

If you are the owner of a project, you can forcefully reclaim all
packages.  If you are assigned a package you can reassign it to the
project's owner.

<a id='download-project'></a>
### Downloading projects

You can download an archive of a project.  It is not possible to
download archives of packages.  The PoCoWeb back-end will then
generate a project archive and you will be asked a location to store
the archive.

The project archive will retain the same structure as the project archive,
that was used to create the project. All corrections are written back into
the original OCR files.

Additionally the archive contains a list of all adaptive tokens of the last
profiler run over the document and a page an line segmented directory structure,
that can directly be used with ocropy.

<a id='profile-project'></a>
### Profiling projects

You can order a document profile for packages and projects.  If you
order a profile for a sub package, the profiler will be run over the
package's parent project.  There can always be only on profiling
process for a document.  More generally there can always be only one
background job (profiling, lexicon extension or automatic post
correction) for any project.

The profiling is done in the background. If the profiling is done,
various helpers are available for the post correction of the profiled project
and its associated packages:
* You will see a list of suspicious words in the manual correction.
* You will see a list of frequent error patterns in the manual correction.
* You will be able to inspect the list of adaptive tokens.

After the profiling is done, you can inspect the list of *adaptive
tokens*.  Adaptive tokens are generated by the profiler.  Whenever a
manually corrected token is found during the profiling, the profiler
tries to match the correction to a known dictionary entry.  If no such
match can be generated, a new adaptive token is generated and inserted
into a temporary dictionary for this document. For this reason
Adaptive tokens are possible candidates for new dictionary entries.

For more information about the profiling and adaptive tokens
see the [profiler paper]() and [profiler paper 2]()

- - -
<a id='post-correction'></a>
## Post correction

After clicking on the __Manual Postcorrection__ tile you will be
directed to the project's or package's first page and you can start to
manually post-correct the first page.  In generally the post
correction is the same for projects an packages.  Every statement
about the correction of projects also applies to the correction of
packages.

<img width="50%" src="assets/images/doc/post_correct_project.png" alt="Page view"/>

Other than PoCoTo the post correction with PoCoWeb is line based.
Generally you correct whole lines of the document and not single tokens.
The page correction view presents the lines of the document and lets
you correct each line individually.

<a id='page-correction'></a>
### Page correction

The page correction view shows the lines of a page of the project.
You can use the forward and backward buttons of the navigation bar to
go to the next, previous first and last pages of the project.  It is
also possible to directly select specific pages of the project.

<img width="25%" src="assets/images/doc/page_navigation.png" alt="Page navigation"/>

For each line the image of the line and the according OCR text are
shown.  Lines and tokens are marked with green if they have been
manually corrected or in orange if the according lines or tokens have
been automatically corrected (either by a script or by the automatic
post-correction system).

If you click in the text you can edit the whole line.  After you have
corrected a line you can click on the
![correct](assets/images/doc/button-correct.png) button to correct the
line and send the correction to the back-end After the whole line has
been corrected, the line it is shown with a green background to mark
it as manually corrected.

<img width="75%" src="assets/images/doc/select_token.png" alt="Token selection"/>

In general you can insert every character into the text field.  There
are no special characters or escape sequences that get by the input
field interpreted.  The only exception are the special escape
sequences of the form `\\u017f` or `\\u{10FFFF}` that can be used to
enter arbitrary Unicode points into the text field. E.g
`Wa\\u017f\\u017fer` gets interpreted as `Waſſer`.  In the unlikely
case that you have to literally insert something like `\\u017f`, you
can use an additional leading `\\` to escape the special
interpretation of the sequence: `\\\\u017f`.

If you are editing a line, you can hit the _Enter_ key to correct the
current line.  You can use the _Up_ and _Down_ arrow keys to navigate
to the previous or next line.

You can click on the __Show line image__ button to see an overview
over the whole line's page image.  On the bottom of the view there is
a button __Set whole page as corrected__ to correct all lines on the
page in on go.

It is also possible to select single tokens in a line, by marking them
with the mouse. If a token is selected, you can choose a correction
candidate for it or list all occurrences of the token in the
[concordance view](#user-content-concordance-view).

<a id='navigation-bar'></a>
### Navigation bar

The navigation bar lets you navigate the pages of you project.  It
stays on the left side of your browser's screen even while you
navigate down the browser page.

Besides the navigation buttons, the navigation bar shows a tab to list
the assumed error patterns (_OCR error patterns_) and error tokens
(_Suspicious words_) of the project.  If the project was profiled
these list assumed errors in the document by the number of their
occurrence or by their common error patterns.  Click one of the
entries to open the [concordance view](#user-content-concordance-view)
of the according token or error patterns.

<img width="25%" src="assets/images/doc/navigation_bar.png" alt="Navigation bar"/>

There is also a list of _special characters_ available.  If you click
on one of the special characters the according Unicode point is
inserted into your clip board (you can use CTRL+v to paste the
character into a text field).  Special characters are all non ASCII
characters encountered in the document.

In the _display settings_ tab you can set options for the displaying
of pages.  These settings are user specific and are only saved in your
browser's local storage and not in the back end.

<a id='concordance-view'></a>
### Concordance view

The concordance view lists tokens in their line context.  These tokens
can be corrected individually or all at once.  The concordance view
never shows manual corrected tokens, since its purpose is the manual
correction of tokens in the project.  If a token gets manually
corrected, it can never be found in the concordance view.

Depending on how you opened the concordance view, you will see a list
of similar words (using suspicious words or the concordance button for
a selected token) or words where the profiler assumes a similar OCR
error pattern for its best interpretation.

<img width="50%" src="assets/images/doc/concordance_view.png" alt="Concordance view"/>

Each matching token is shown in a blue outline. All matched tokens can
be individually corrected using profiler suggestions or manual
editing.  It is also possible to globally batch-correct the shown
tokens using the correction bar at the top of the concordance
view. All inputs allow the insertion of Unicode escape codes.

After one or more tokens have been corrected, a new batch of tokens is
shown (if there are more to show).  It is possible to set the number
of tokens per page in the _display settings_ tab of the [page
correction](#user-content-page-correction).

- - -
<a id='installation'></a>
## Installation
PoCoWeb is open source software and is distributed under the
[Apache 2.0](LICENSE) license.
You can download the source code from its
[github repository](http://github.com/cisocrgroup/pocoweb).
Installation instructions can be found in the project's
[README](https://github.com/cisocrgroup/pocoweb/blob/master/README.md)
file.

PoCoWeb can be deployed using
[docker-compose](https://docs.docker.com/compose/).  The according
Docker files can be found in the projects's source directory.

- - -
<a id='pool'></a>
## Ground-truth pool
It is possible to download an archive of all corrected lines with
their respective image files.  This pool contains line segmented
ground-truth data and is suitable to be used as training material for
OCR.

Adminstrators can download a pool containing all their [owned
projects](#user-content-api-get-user-pool) or a [global
pool](#user-content-api-get-global-pool) containing data of all
projects in PoCoWeb.  If for some reason a project should not be part
of this global pool it is possible to set the variable `pooled` to
`false` in the projects's settings.

The pool is a zipped archive that contains the line segmented
correction data in a directory structure that is ordered by
project, page ids and line ids:
```
corpus
├── year-author_name1_book_title1.json
├── year-author_name1_book_title1
│   ├── pageid1
│   │   ├── lineid1.gt.txt
│   │   ├── lineid1.txt
│   │   ├── lineid1.png
│   │   ├── lineid2.gt.txt
│   │   ├── lineid2.txt
│   │   └── lineid2.png
│   └── pageid2
│       ├── lineid1.gt.txt
│       ├── lineid1.txt
│       ├── lineid1.png
│       ├── lineid2.gt.txt
│       ├── lineid2.txt
│       └── lineid2.png
├── year-author_name2_book_title2.json
└── year-author_name2_book_title2
    ├── pageid1
    │   ├── lineid1.gt.txt
    │   ├── lineid1.txt
    │   ├── lineid1.png
    │   ├── lineid2.gt.txt
    │   ├── lineid2.txt
    │   └── lineid2.png
    └── pageid2
        ├── lineid1.gt.txt
        ├── lineid1.txt
        ├── lineid1.png
        ├── lineid2.gt.txt
        ├── lineid2.txt
        └── lineid2.png
```

For each project directory, a small JSON-formatted info file is
included. The file has the following layout:

```json
{
	"Author":"author's name",
	"Title":"book's title",
	"Description":"book's description",
	"OwnerEmail":"owner's email",
	"Language": "book's profiler language",
	"ID":3,
	"Year":1900,
	"OCRTXTFiles": [
		"..."
	],
	"GTIMGFiles": [
		"..."
	],
	"GTTXTFiles": [
		"..."
	],
	"Pooled":true
}
```

- - -
<a id='overview-services'></a>
## Services
PoCoWeb is composed from a number of interdependend services:
* nginx serves images, web-content and redirectes API calls
* pcwauth authentificates API requests and forwards them to the various other services
* mysql holds all database tables
* PoCoWeb manages project, packages and the up- and download
* www-data is a directory (volume) that holds the frontend
* project-data is a directory (volume) that holds the project files
* db-data is a directory (volume) that holds the database files
* ocr-data is a direcotry (volume) that holds the global ocr models
* pcwuser handles user management
* pcwprofiler handles profiling of projects
* pcwpostcorrection handles the automatic postcorrection
* pcwpkg handles splitting and assignment of packages
* pcwpool handles download of [pool](#user-content-pool) data
![Service Overview](assets/images/doc/overview.svg "PoCoWeb service overview")

- - -
<a id='rest-api'></a>
## REST API

The REST API documentation lists the available URLs of PoCoWeb's REST
API.  This API is used by the front-end to implement the
post-correction.  It can also be used to automate some aspects of
PoCoWeb (creation of users, splitting of packages etc).

Each entry gives the request method (GET, POST or DELETE) in square
brackets, the request path and the layout of the response data is
listed.  Additionally for POST requests the expected layout of the
POST data is listed.  You have to replace the parameters in the
request path by valid ids (otherwise the API returns `404 Not Found`):
 * `uid` references a valid user id.
 * `pid` references a valid project or package id.
 * `pageid` references a valid page id in a project or package.
 * `lid` references a valid line id on a page.
 * `tid` references a valid token id on a line.

You have to be logged in with a valid user account
(see [Authorization](#user-content-authorization) below).

All URLs in the API documentation reference a relative path starting
with `rest/...`.  If you want to use external tools such as `curl` to
access the API, you have to use the according absolute URL.  So for
example if PoCoWeb runs on your local host you can access the API
using `http://localhost/rest/...`.  E.g. to get the API's version (see
[Version](#user-content-api-get-version) below) using `curl` you have
to execute the following command: `curl
http://localhost/rest/api-version`.

---
<a id='authorization'></a>
### Authentification

You have to be authenticated for most of the API calls.  If you are
not authenticated the API will return `401 Not Authenticated` with no
response data.  If you try to access project or package data that you
are not allowed to access, the API will `403 Forbidden` instead.

In order to authenticate you first have to
[login](#user-content-api-post-login) with a valid user account at the
back-end.  The login will return an unique session id token.  You have
to send this session token using an additional `auth=id` parameter for
each API call.

For example if your session id is `my-session-id` you have to append
the `auth=my-session-id` to any URL. So for example using
[curl](https://curl.haxx.se/), you would need to call `curl
http://localhost/rest/books/pid/pages/pageid?auth=my-session-id`.

You can use curl together with [jq](https://stedolan.github.io/jq/) to
automate the PoCoWeb backend.  So if you want to list all projects and
package ids of a particular user, you could issue the following
commands (see the [login](#user-content-api-post-login) and [list
projects](#user-content-api-get-books) documentation for more
information):

```sh
# login and extract the auth token
$ export AUTH=$(curl -s -d '{"password":"pass","email":"user@example.com"}' "https://localhost/rest/login" | jq -r '.auth')
# list project and package ids
$ curl -s "https://localhost/rest/books?auth=$AUTH" | jq '.books[] | .projectId'
```

---
<a id='api-get-version'></a>
### GET rest/api-version

Get the semantic version of the back-end instance.
* No [authorization](#user-content-authorization) is required.

#### Response data
```json
{
  "version": "1.2.3" // PoCoWeb API version
}
```

---
<a id='api-post-login'></a>
### POST rest/login
You have to log in with a valid user account to use most of the API calls
(see [authorization](#user-content-authorization) above).
No [authorization](#user-content-authorization) is required.

After a successfully login you can use the returned session id in the
`Authorization` header of your future API calls.

#### POST data
```json
{
  "email": "user email",
  "password": "password"
}
```

#### Response data
```json
{
  "auth": "auth-token",
  "expires": 1571663749,
  "user": {
	"id": 42,
	"name": "user-name",
	"email": "user-email",
	"insitute": "user-institute",
	"admin": true|false
  }
}
```

---
<a id='api-get-login'></a>
### GET rest/login
Check if logged in and get the settings of the logged in user account.
* [Authorization](#user-content-authorization) is required.

#### Response data
```json
{
  "id": 42,
  "name": "user-name",
  "email": "user-email",
  "insitute": "user-institute",
  "admin": true|false
}
```

---
<a id='api-post-users'></a>
### POST rest/users
Create a new user account.
* [Authorization](#user-content-authorization) is required.
* Only administrators can create new user accounts.
* If the `"password"` field in the post data is omitted (or is set to
  `""` explicitly), the user's password is not changed.

#### POST data
```json
{
	"user": {
	    "id": 0,
		"name": "user-name",
		"email": "user's unique email",
		"insitute": "user-institute",
		"admin": true|false
	},
	"password": "new user's password"
}
```

#### Response data
```json
{
  "id": 42,
  "name": "user-name",
  "email": "user-email",
  "insitute": "user-institute",
  "admin": true|false
}
```

---
<a id='api-get-users'></a>
### GET rest/users
Get a list of all users in the system.
* [Authorization](#user-content-authorization) is required.
* Only administrators can list all user accounts.

#### Response data
```json
{
  "users": [
    {
      "id": 42,
      "name": "user-name-1",
      "email": "user-email-1",
      "insitute": "user-institute-1",
      "admin": true|false
    },
    {
      "id": 42,
      "name": "user-name-2",
      "email": "user-email-2",
      "insitute": "user-institute-2",
      "admin": true|false
    },
	...
  ]
}
```


---
<a id='api-post-users-uid'></a>
### PUT rest/users/`uid`
Update the account settings of an user with an id of `uid`.
* [Authorization](#user-content-authorization) is required.
* Administrators can update any normal user account.
* Every user can change his own user account.
* You cannot change the type (administrator or normal) of an user account.

#### POST data
```json
{
	"user": {
	    "id": 42,
		"name": "user-name",
		"pass": "password",
		"email": "user-email",
		"insitute": "user-institute"
	},
	"password": "user's new password"
}
```
You can leave out the password or use the empty string,
if you do not want to update the user's password. It is
not possible to change a user's id.

#### Response data
```json
{
  "id": 42,
  "name": "updated-user-name",
  "email": "updated-user-email",
  "insitute": "updated-user-institute",
  "admin": true|false
}
```

---
<a id='api-get-users-uid'></a>
### GET rest/users/`uid`

Get the settings of an user account with an id of `uid`.
* [Authorization](#user-content-authorization) is required.
* Administrators can get the settings of any normal user account.
* Every user can get the settings of her own user account.

#### Response data
```json
{
  "id": 42,
  "name": "user-name",
  "email": "user-email",
  "insitute": "user-institute",
  "admin": true|false
}
```

---
<a id='api-delete-users-uid'></a>
### DELETE rest/users/`uid`
Delete the user account of the user with an id of `uid`.
* [Authorization](#user-content-authorization) is required.
* Administrators can delete any user account.
* Every user can delete her own user account.

If an user account is deleted, any projects and or packages, that the
user owned are deleted.  Since packages just define a subset of a
project, the deletion of packages deletes the package information, but
not the project information.  It is save to delete a user that still
owns a package.  Note that if a project is deleted, all associated
packages are deleted as well.

---
<a id='api-get-books'></a>
### GET rest/books
Get all books of an user account.
* [Authorization](#user-content-authorization) is required.

#### Response data
```json
{
  "books": [
    {
      "author": "book-author",
      "title": "book-title",
      "year": 1234,
      "language": "language",
      "profilerUrl": "profiler-url|local",
      "bookId": 27,
      "projectId": 42,
      "pages": 100,
      "pageIds": [15,18,20,27,...],
	  "histPatterns": "t:th,ei:ey",
	  "pooled": true,
	  "status": {
		"profile": false,
		"extended-lexicon": false,
		"post-corrected": false
	  },
      "isBook": true|false
    },
    ...
  ]
}
```

---
<a id='api-post-books'></a>
### POST rest/books
Create a new project.
* [Authorization](#user-content-authorization) is required.
* Only administrators can upload new projects.
* Upload [project archive](#user-content-project-archives).
* You can add optional query parameters to set the book's metadata.
* Use `Content-Type: application/zip` in the request header.

#### POST data
Zipped content of the [project archive](#user-content-project-archives).

#### Query parameters
All query parameters are optional.  Missing parameters are set to the
empty string `""`, `0` or `false` respectively.
* `author=book-author`
* `title=book-title`
* `description=description`
* `language=language`
* `profilerUrl=url`
* `histPatterns=[m1:h1[,m2:h2]...]`
* `year=1900`
* `pooled=true`

#### Response data
```json
{
  "author": "book-author",
  "title": "book-title",
  "description": "book's description",
  "year": 1234,
  "language": "language",
  "profilerUrl": "profiler-url|local",
  "bookId": 27,
  "projectId": 27,
  "pages": 100,
  "pageIds": [15,18,20,27,...],
  "histPatterns": "t:th,ei:ey",
  "pooled": true,
  "status": {
	"profile": false,
	"extended-lexicon": false,
	"post-corrected": false
  },
  "isBook": true
}
```

---
<a id='api-get-books-pid'></a>
### GET rest/books/`pid`
Get the content of a project or package.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access the project's or
  package's data.

#### Response data
```json
{
  "author": "book-author",
  "title": "book-title",
  "description": "book's description",
  "year": 1234,
  "language": "language",
  "profilerUrl": "profiler-url|local",
  "bookId": 27,
  "projectId": 42,
  "pages": 100,
  "pageIds": [15,18,20,27,...],
  "status": {
	"profile": true,
	"extended-lexicon": false,
	"post-corrected": false
  },
  "histPatterns": "t:th,ei:ey",
  "pooled": true,
  "isBook": true|false
}
```

---
<a id='api-put-books-pid'></a>
### PUT rest/books/`pid`
Update the meta data of a project.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project can update the project's data.
* Since only adminstrators can upload and therefore own projects, only
  administrator can change a project's metadata.
* Fields can be ommitted.

#### POST data
```json
{
  "author": "book-author",
  "title": "book-title",
  "year": 1234,
  "language": "language",
  "histPatterns": "[m1:h1[,m2:h2]...]",
  "profilerUrl": "profiler-url|local",
  "histPatterns": "t:th,ei:ey",
  "pooled": true
}
```

---
<a id='api-delete-books-pid'></a>
### DELETE rest/books/`pid`
Delete a project.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project can delete a project.

If a project is deleted, all associated packages are deleted as well.
It is not possible to delete a package.  The deletion remove all
database entries of the projects and all associated packages.  In
addition to this the page and line images and all remaining project
archives are removed from the server.

---
<a id='api-get-books-pid-statistics'></a>
### GET rest/books/`pid`/statistics
Get correction statistics for a project or package.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access the project's or
  package's statistics.

#### Response data
```json
{
	"bookId":50,
	"projectId":50,
	"lines":1211,
	"corLines":1,
	"tokens":12045,
	"corTokens":1,
	"ocrCorTokens":0,
	"acTokens":0,
	"acCorTokens":0
}
```

---
<a id='api-post-books-pid-split'></a>
### POST rest/pkg/split/books/`pid`
Split a [project into packages](#user-content-projects).
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project can split a project into packages.

#### Post data
```json
{
  "userIds": [1, 2, 3],
  "random": true|false
}
```
* The mandatory argument `ids` specifies an not empty array of user
  ids. The split route creates that many packages and assignes them to
  the given users.  User-IDs in the `ids` array need not be unique.
* The optional argument `random` specifies if the pages of the
  packages should be generated at random or sequential.  If omitted
  `random=false` is assumed.

#### Response data
```json
{
  "bookId": 19
  "projects": [
    {
	  "projectId": 38,
	  "owner": 17,
	  "pageIds": [15, 16, ...]
	},
	...
  ]
}
```

---
<a id='api-get-books-pid-assign'></a>
### GET rest/pkg/assign/books/`pid`
Assign a packages to users.
* [Authorization](#user-content-authorization) is required.
* Only an administrator can assign packages to different users.
* Only the owner of a package can assign it back to its original
  owner.

#### Query parameters
* The id of the user to which the package should be assigned,
  must be given with the `assignto=user-id` parameter.
* If the parameter `assignto` is ommited, the package is assigned to
  the original owner of the package (the owner of the associated
  project).

---
<a id='api-get-books-pid-takeback'></a>
### GET rest/pkg/takeback/books/`pid`
Take all packages of a project back.  All packages beloning to the
project are reasigned to the project's owner.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project can take packages back.

---
<a id='api-get-books-pid-download'></a>
### GET rest/books/`pid`/download
Download a project.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project can delete a project.

This API call generates a new project archive and returns the relative
URL of the new archive.  The new project contains all original OCR and
image files with all corrections.  Additionally the archive contains a
file called `adaptive_tokens.txt` as well as a `ocropus-book`
directory structure.

The file `adaptive_tokens.txt` contains the [adaptive token
set](#user-content-profile-project) of the various profiler runs (if
any). This file can be empty, if no profiling was used or if the
profiler could not find any adaptive tokens.

The `ocorpus-book` directory contains an [ocropy directory
structure](#user-content-project-archives) that can directly be used
for model training with ocropy.  If the project archive was already a
ocropy directory structure, only the according ground truth files are
updated.

#### Response data
```json
{
  "archive": "relative download url of project archive",
}
```

---
<a id='api-get-books-pid-search'></a>
### GET rest/books/`pid`/search
Search for a token, OCR-error pattern or auto corrected token in a
project or package with id `pid`.  The search excludes corrected
tokens.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can search for tokens or
  error patterns.

#### Query parameters
* The query is given with the `q=query` parameter. This parameter is
  mandatory.
* The optional parameter `t=token|pattern|ac|regex` specifies the type
  of the search (default: `token`):
  * `token` specifies a keyword search for the query
  * `pattern` specifies a (OCR) pattern query
  * `ac` specifies a search for auto corrected tokens
  * `regex` specifies that the queries are regular expressions
* The optional parameter `max=n` sets the maximal number of returned
  hits (default: `max=50`).
* The optional parameter `skip=n` sets the number of hits to skip
  (default: `skip=0`).

#### Response data
```json
{
  "projectId": 27,
  "bookId": 17,
  "type": "search type: token|pattern|ac|regex",
  "skip": 8,
  "max": 14,
  "totalCount": 1000,
  "matches": {
	"query1": [{
	   "totalCount": 30,
	   "line": {
          "lineId": 13,
          "pageId": 38,
          "projectId": 27,
          "imgFile": "path/to/line/image/file",
          "cor": "corrected content",
          "ocr": "ocr content",
          "cuts": [1, 3, 5, 7, 9, 11, ...],
          "confidences": [0.1, 0.1, 0.9, 1.0, ...],
          "averageConfidence": 0.5,
          "isManuallyCorrected": true|false,
          "isAutomaticallyCorrected": true|false,
          "box": {
            "left": 1,
            "right": 2,
            "top": 3,
            "bottom": 4,
            "width": 1,
            "height": 1
          }
        }
		},...],
		"matches": [{
          "projectId": 27,
          "pageId": 38,
          "lineId": 13,
          "tokenId": 17,
          "offset": 8,
          "isManuallyCorrected": true|false,
          "isAutomaticallyCorrected": true|false,
          "cor": "corrected content of token",
          "ocr": "ocr content of token",
          "averageConf": 0.3,
          "isNormal": true|false,
          "box": {
            "left": 1,
            "right": 2,
            "top": 3,
            "bottom": 4,
            "width": 1,
            "height": 1
          }},...
		]
	}
}
```

---
<a id='api-get-books-pid-pages-pageid'></a>
### GET rest/books/`pid`/pages/`pageid`
Get the content of a page with id `pageid` of a package or project
with id `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can read its pages.

#### Response data
```json
{
  "pageId": 38,
  "projectId": 27,
  "ocrFile": "name of ocr file",
  "imgFile": "name of image file",
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  },
  "lines": [
    {
      "lineId": 13,
      "pageId": 38,
      "projectId": 27,
      "imgFile": "path/to/line/image/file",
      "cor": "corrected content",
      "ocr": "ocr content",
      "cuts": [1, 3, 5, 7, 9, 11, ...],
      "confidences": [0.1, 0.1, 0.9, 1.0, ...],
      "averageConfidence": 0.5,
      "isManuallyCorrected": true|false,
      "isAutomaticallyCorrected": true|false,
      "box": {
        "left": 1,
        "right": 2,
        "top": 3,
        "bottom": 4,
        "width": 1,
        "height": 1
      }
      "tokens": [
        {
          "projectId": 27,
          "pageId": 38,
          "lineId": 13,
          "tokenId": 17,
          "offset": 8,
          "isManuallyCorrected": true|false,
          "isAutomaticallyCorrected": true|false,
          "cor": "corrected content of token",
          "ocr": "ocr content of token",
          "averageConf": 0.3,
          "isNormal": true|false,
          "box": {
            "left": 1,
            "right": 2,
            "top": 3,
            "bottom": 4,
            "width": 1,
            "height": 1
          },
        },
        ...
      ]
	}
  ]
}
```

---
<a id='api-delete-books-pid-pages-pageid'></a>
### DELETE rest/books/`pid`/pages/`pageid`
Delete the pages with `pageid` from the project with id `pid`.
* [Authorization](#user-content-authorization) is required.
* Only pages from projects can be deleted.  You cannot delete pages
  from packages.
* Only the owner of a project can delete it.

---
<a id='api-get-books-pid-pages-first'></a>
### GET rest/books/`pid`/pages/first
Get the first page of a project or package with id `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can read its pages.

#### Response data
```json
{
  "pageId": 38,
  "projectId": 27,
  "ocrFile": "name of ocr file",
  "imgFile": "name of image file",
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  },
  "lines": [
    {
      "lineId": 13,
      "pageId": 38,
      "projectId": 27,
      "imgFile": "path/to/line/image/file",
      "cor": "corrected content",
      "ocr": "ocr content",
      "cuts": [1, 3, 5, 7, 9, 11, ...],
      "confidences": [0.1, 0.1, 0.9, 1.0, ...],
      "averageConfidence": 0.5,
      "isManuallyCorrected": true|false,
      "isAutomaticallyCorrected": true|false,
      "box": {
        "left": 1,
        "right": 2,
        "top": 3,
        "bottom": 4,
        "width": 1,
        "height": 1
      }
    },
    ...
  ]
}
```
---
<a id='api-get-books-pid-pages-last'></a>
### GET rest/books/`pid`/pages/last
Get the last page of a project or package with id `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can read its pages.

#### Response data
```json
{
  "pageId": 38,
  "projectId": 27,
  "ocrFile": "name of ocr file",
  "imgFile": "name of image file",
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  },
  "lines": [
    {
      "lineId": 13,
      "pageId": 38,
      "projectId": 27,
      "imgFile": "path/to/line/image/file",
      "cor": "corrected content",
      "ocr": "ocr content",
      "cuts": [1, 3, 5, 7, 9, 11, ...],
      "confidences": [0.1, 0.1, 0.9, 1.0, ...],
      "averageConfidence": 0.5,
      "isManuallyCorrected": true|false,
      "isAutomaticallyCorrected": true|false,
      "box": {
        "left": 1,
        "right": 2,
        "top": 3,
        "bottom": 4,
        "width": 1,
        "height": 1
      }
    },
    ...
  ]
}
```

---
<a id='api-get-books-pid-pages-pageid-next-n'></a>
### GET rest/books/`pid`/pages/`pageid`/next/`n`
Get the `n`-th next page of a page with id `pageid` of a project or package with id `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can read its pages.

#### Response data
```json
{
  "pageId": 38,
  "projectId": 27,
  "ocrFile": "name of ocr file",
  "imgFile": "name of image file",
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  },
  "lines": [
    {
      "lineId": 13,
      "pageId": 38,
      "projectId": 27,
      "imgFile": "path/to/line/image/file",
      "cor": "corrected content",
      "ocr": "ocr content",
      "cuts": [1, 3, 5, 7, 9, 11, ...],
      "confidences": [0.1, 0.1, 0.9, 1.0, ...],
      "averageConfidence": 0.5,
      "isManuallyCorrected": true|false,
      "isAutomaticallyCorrected": true|false,
      "box": {
        "left": 1,
        "right": 2,
        "top": 3,
        "bottom": 4,
        "width": 1,
        "height": 1
      }
    },
    ...
  ]
}
```

---
<a id='api-get-books-pid-pages-pageid-prev-n'></a>
### GET rest/books/`pid`/pages/`pageid`/prev/`n`
Get the `n`-th previous page of a page with id `pageid` of a project
or package with id `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can read its pages.

#### Response data
```json
{
  "pageId": 38,
  "projectId": 27,
  "ocrFile": "name of ocr file",
  "imgFile": "name of image file",
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  },
  "lines": [
    {
      "lineId": 13,
      "pageId": 38,
      "projectId": 27,
      "imgFile": "path/to/line/image/file",
      "cor": "corrected content",
      "ocr": "ocr content",
      "cuts": [1, 3, 5, 7, 9, 11, ...],
      "confidences": [0.1, 0.1, 0.9, 1.0, ...],
      "averageConfidence": 0.5,
      "isManuallyCorrected": true|false,
      "isAutomaticallyCorrected": true|false,
      "box": {
        "left": 1,
        "right": 2,
        "top": 3,
        "bottom": 4,
        "width": 1,
        "height": 1
      }
    },
    ...
  ]
}
```

---
<a id='api-get-books-pid-pages-pageid-lines-lid'></a>
### GET rest/books/`pid`/pages/`pageid`/lines/`lid`
Get the line with id `lid` of a page with id `pageid` from a project
or package with id `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can read its lines.

#### Response data
```json
{
  "lineId": 13,
  "pageId": 38,
  "projectId": 27,
  "imgFile": "path/to/line/image/file",
  "cor": "corrected content",
  "ocr": "ocr content",
  "cuts": [1, 3, 5, 7, 9, 11, ...],
  "confidences": [0.1, 0.1, 0.9, 1.0, ...],
  "averageConfidence": 0.5,
  "isManuallyCorrected": true|false,
  "isAutomaticallyCorrected": true|false,
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  }
}
```

---
<a id='api-post-books-pid-pages-pageid-lines-lid'></a>
### [POST/PUT] rest/books/`pid`/pages/`pageid`/lines/`lid`
Correct line `lid` in page `pageid` of project or package `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can correct lines.
* It is not possible to overwrite manually confirmed corrections with
  not manually generated corrections.  In this case `409 Conflict` is
  returned.
* If an invalid `t` parameter is posted `400 Bad Request` is
  returned.

#### Query parameters
You can set the type of the correction using `t=manual|automatic` as
query paramter in the url.  Setting `t=reset`, resets the line to its
original ocr data. This deletes any automatic and/or manual
corrections.  It is also possible to use `t=ocr` (see
[below](#user-content-api-post-books-pid-pages-pageid-lines-lid-ocr)). If
`t` is missing, `"t=automatic"` is assumed.

#### Post data
The correction for the line is set using the mandatory `correction`
parameter in the post data for the request.  The optional `type`
parameter can be used to set the type of the correction.  If missing,
`"automatic"` is assumed.

```json
{
  "correction": "corrected line",
}
```

---
<a id='api-post-books-pid-pages-pageid-lines-lid-ocr'></a>
### [POST/PUT] rest/books/`pid`/pages/`pageid`/lines/`lid`?t=ocr
Update the underlying ocr information of a line.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can update lines.
* Be aware that all corrections for the given line are lost.
* If an invalid `t` parameter is posted `400 Bad Request` is
  returned.

#### Query parameters
Set the query parameter `t=ocr`.

#### Post data
The data to update the line.  The number of elements in `cuts` must be
the same as the number of unicode points in `ocr`.  Otherwise a `400
Bad Request` is returned.  Both parameters `confidences` and
`imageData` can be omitted.

```json
{
  "ocr": "ocr for line",
  "cuts": [10, 20, 30],
  "confidences": [0.9, 0.8, 1.0],
  "imageData": "base 64 encoded image data"
}
```

#### Response data
```json
{
  "lineId": 13,
  "pageId": 38,
  "projectId": 27,
  "imgFile": "path/to/line/image/file",
  "cor": "corrected content",
  "ocr": "ocr content",
  "cuts": [1, 3, 5, 7, 9, 11, ...],
  "confidences": [0.1, 0.1, 0.9, 1.0, ...],
  "averageConfidence": 0.5,
  "isManuallyCorrected": true|false,
  "isAutomaticallyCorrected": true|false,
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  }
}
```

---
<a id='api-delete-books-pid-pages-pageid-lines-lid'></a>
### DELETE rest/books/`pid`/pages/`pageid`/lines/`lid`
Delete line `lid` in page `pageid` of project with id `pid`.
* [Authorization](#user-content-authorization) is required.
* You can only delete lines from projects.  You cannot delete lines
  from packages.
* Only the owner of a project can delete a line.

---
<a id='api-get-books-pid-pages-pageid-lines-lid-tokens-tid'></a>
### GET rest/books/`pid`/pages/`pageid`/lines/`lid`/tokens/`tid`
Get a token (or slice) of a token in a line starting at position
`tid`.  The position specifies the offset of the token in the line
(counting unicode code-points).
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access its tokens.

#### Query parameters
An optional parameter `len=n` can be specified to get a specific slice
of the line starting at position `tid` and ending at position
`tid+len-1` (counting unicode code-points).  If `len` is omitted an
according token (ending at the first encountered whitespace character
after `tid` or the end of the line) is returned.

#### Response data
```json
{
  "lineId": 13,
  "pageId": 38,
  "projectId": 27,
  "bookId": 27,
  "tokenId": 77,
  "offset": 77,
  "cor": "corrected content",
  "ocr": "ocr content",
  "cuts": [1, 3, 5, 7, 9, 11, ...],
  "confidences": [0.1, 0.1, 0.9, 1.0, ...],
  "averageConfidence": 0.5,
  "isManuallyCorrected": true|false,
  "isAutomaticallyCorrected": true|false,
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  }
}
```

--- <a id='api-post-books-pid-pages-pageid-lines-lid-tokens-tid'></a>
### [POST/PUT] rest/books/`pid`/pages/`pageid`/lines/`lid`/tokens/`tid`
Correct words/slices `tid` in line `lid` in page `pageid` of project
or package `pid`.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can correct its tokens.
* It is not possible to overwrite manually confirmed corrections with
  not manually generated corrections.  In this case `409 Conflict` is
  returned.
* If an invalid `type` parameter is posted `400 Bad Request` is
  returned.

#### Query parameters
You can set the type of the correction using `t=manual|automatic` as
query paramter in the url.  Setting `t=reset` resets the token to its
original OCR data.  This removes any automatic and/or manual
corrections for the token.  If `t` is missing, `"t=automatic"` is
assumed.  An optional parameter `len=n` can be specified to correct a
specific slice of the line starting at position `tid` and ending at
position `tid+len-1` (counting unicode code-points and starting at
index 0).  If `len` is omitted an according token (ending at the first
encountered whitespace character after `tid` or the end of the line)
is corrected.

#### Post data
The correction for the word/slice is set using the mandatory
`correction` parameter in the post data for the request.

```json
{
  "correction": "corrected line",
}
```

#### Response data
```json
{
  "lineId": 13,
  "pageId": 38,
  "projectId": 27,
  "tokenId": 77,
  "offset": 10,
  "cor": "corrected content",
  "ocr": "ocr content",
  "cuts": [1, 3, 5, 7, 9, 11, ...],
  "confidences": [0.1, 0.1, 0.9, 1.0, ...],
  "averageConfidence": 0.5,
  "isManuallyCorrected": true|false,
  "isAutomaticallyCorrected": true|false,
  "box": {
    "left": 1,
    "right": 2,
    "top": 3,
    "bottom": 4,
    "width": 1,
    "height": 1
  }
}
```

--- <a id='api-get-books-pid-profile'></a>
### GET rest/profile/books/`pid`
Get profiler suggestions after the according profiling
[job](#user-content-api-get-jobs) has finished.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access the profiler
  information.
* Suggestions for ocr tokens are only available if the project has
  been profiled.

#### Query parameters
Optionally you can add `q=q1&q=q2&...` parameters to limit the
suggestions to the given queries.  If no query parameters are set, the
whole profile for each type in the document is returned.

#### Response data
```json
{
	"projectId": 13,
	"bookId": 37,
	"ocr1": [
		{
			"token": "ocr1",
			"suggestion": "correction suggestion",
			"modern": "modern lexicon entry",
			"dict": "dictionary name",
			"distance": 2,
			"id": 13446,
			"weight": 0.3,
			"top": true,
			"ocrPatterns": ["pat1:pat2:pos2", "pat3:pat4:pos2"],
			"histPatterns": ["pat5:pat6:pos3"]
		},
		{
			"token": "ocr1",
			"suggestion": "correction suggestion",
			"modern": "modern lexicon entry",
			"dict": "dictionary name",
			"distance": 1,
			"id": 13446,
			"weight": 0.01,
			"top": false,
			"ocrPatterns": ["pat1:pat2:pos2"],
			"histPatterns": ["pat3:pat4:pos2","pat5:pat6:pos3"]
		},
	    ...
	]
}
```

---
<a id='api-post-books-pid-profile'></a>
### POST rest/profile/books/`pid`
Request to profile the project with an id `pid` or request to profile
the original project of a package with an id `pid`.  The request
starts the profiling as background [job](#user-content-api-get-jobs)
and returns the according job information.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can start a profiling job for
  the (package's base-) project.
* Returns the [job](#user-content-api-get-jobs) id for the started
  profiling job.

#### Post data
The profiling endpoint expects a list of tokens that are used as
additional lexicon entries for the profiling.  It is possible to just
send an empty tokens list if no additional lexicon entries should be
used for the profiling.

```json
{
	"tokens": ["additional lexicon token 1", "additional lexicon token 2"]
}
```

#### Response data
```json
{
	"id": 13
}
```

---
<a id='api-get-books-pid-suspicious'></a>
### GET rest/profile/suspicious/books/`pid`
Get the `suspicious` words for the given project and/or package.
Suspicious words are words in the project for which the profiler
guesses an underlying OCR-error.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access the profiler
  information.
* Suspicious words are only available if the project has been
  profiled.

#### Response data
```json
{
  "projectId": 19,
  "bookId": 19,
  "counts": {
	  "suspicious-word#1": 15,
	  "suspicious-word#2": 134,
	  ...
  }
}
```

---
<a id='api-get-profiler-languages'></a>
### GET rest/profile/languages
Get the available languages of a language profiler.
* No [Authorization](#user-content-authorization) is required.

#### Query parameters
* The optional parameter `url=profiler-url` specifies the URL of the profiler.
Use `local` to use the local profiler. If omitted, `url=local` is assumed.

#### Response data
```json
{
  "url": "profiler-url|local",
  "languages": ["language1", "language2", ...]
}
```

---
<a id='api-post-books-pid-adaptive-tokens'></a>
### GET rest/profile/adaptive/books/`pid`
Get a list of the adaptive tokens of the last profiler run.  Adaptive
tokens are tokens that where manually corrected in the
project/package.  They are used as hints for the profiler.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project can access the adaptive token set of the project.

#### Response data
```json
{
  "projectId": 27, // package id
  "bookId": 27, // project id
  "adaptiveTokens": ["token1", "token2", ...] // list of adaptive tokens
}
```

---
<a id='api-get-el'></a>
### GET rest/postcorrect/le/books/`pid`
Get the extended lexicon for the project.  The extendend lexicon is
available after the [job](#user-content-api-get-jobs) for the
according [post request](#user-content-api-post-el) has finished.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access the extended
  lexicon.

#### Response data
```json
{
	"bookId": 13,
	"projectId": 42,
	"yes": {
		"foo": 3,
		"bar": 8
	},
	"no": {
		"baz": 8
	}
}
```

---
<a id='api-put-el'></a>
### PUT rest/postcorrect/le/books/`pid`
Update extended lexicon for the project.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access the extended
  lexicon.

#### Put data
```json
{
	"bookId": 13,
	"projectId": 42,
	"yes": {
		"foo": 3,
		"bar": 8
	},
	"no": {
		"baz": 8
	}
}
```

#### Response data
```json
{
	"bookId": 13,
	"projectId": 42,
	"yes": {
		"foo": 3,
		"bar": 8
	},
	"no": {
		"baz": 8
	}
}
```

---
<a id='api-post-el'></a>
### POST rest/postcorrect/le/books/`pid`
Start the [job](#user-content-api-get-jobs) to generate the extended
lexicon.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can start the job.

#### Response data
```json
{
	"id": 31
}
```

---
<a id='api-get-rrdm'></a>
### GET rest/postcorrect/books/`pid`
Get the post-correction information.  The post-correction information
available after the [job](#user-content-api-get-jobs) for the
according [post request](#user-content-api-post-rrdm) has finished.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can access the
  post-correction information.

#### Response data
```json
{
	"bookId": 13,
	"projectId": 42,
	"corrections": {
		"idstr": {
			"bookID": 3,
			"projectID": 5,
			"pageID": 15,
			"lineID": 8,
			"tokenID": 0,
			"normalized": "normalized token",
			"cor": "correction (unnormalized)",
			"ocr": "original (unnormalized) ocr",
			"taken": true
		}
	}
}
```

---
<a id='api-post-rrdm'></a>
### POST rest/postcorrect/books/`pid`
Start the [job](#user-content-api-get-jobs) to generate the post
correction.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can start the job.

#### Response data
```json
{
	"id": 31
}
```

---
<a id='api-post-retrain'></a>
### POST rest/postcorrect/train/books/`pid`
Start the [job](#user-content-api-get-jobs) to retrain the
post-correction model.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can start the job.

#### Response data
```json
{
	"id": 31
}
```

---
<a id='api-get-global-pool'></a>
### GET rest/pool/global
Download the global ground-truth [pool](#user-content-pool) of
corrected lines in all projects.
* [Authorization](#user-content-authorization) is required.
* Only adminstrators can download the global pool.

---
<a id='api-get-user-pool'></a>
### GET rest/pool/user
Download the user's ground-truth [pool](#user-content-pool) of
corrected lines in all projects of a user.
* [Authorization](#user-content-authorization) is required.
* Only adminstrators can download the user's pool.
* The archive contains data from all projects the user owns;
  the `pooled` setting in the projects is ignored.

---
<a id='api-get-jobs'></a>
### GET rest/jobs/`jobid`
Get the status of a job for the given job-id.
* [Authorization](#user-content-authorization) is required.
* Only the owner of a project or package can see the job.
* If the job failed `500 Internal Server Error` is returned.

#### Query parameters
You can specify an optional `q=job-name` parameter to check the status
of a specific running job (such as predict, train, profiler,
postcorrection) with the according id.

#### Response data
```json
{
	"jobId": 8,
	"bookId": 13,
	"statusId": 1,
	"statusName": "failed|running|done",
	"jobName": "name of job",
	"Timestamp": 0
}
```

<!--  LocalWords:  PoCoWeb
 -->
