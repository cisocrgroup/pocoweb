# Pocoweb

Pocoweb is a web based postcorrection system for OCRed historical documents.
It is loosely based on [PoCoTo](https://github.com/cisocrgroup/PoCoTo).

Users can upload their OCRed documents and correct them in the web frontend.
The corrected documents can then be downloaded and further processed.
Pocoweb understands various OCR formats that can be used.
Any corrections are written back into the original format of the documents.
This makes it possible to integrate the manual postcorrection of OCRed documents
into other digitalisation workflows.

As PoCoTo, Pocoweb is connected with a language
[profiler](https://github.com/cisocrgroup/Profiler)
that enables semi-automatic correction of historical documents.
The profiler is able to differentiate between historical spelling variation
and real OCR errors in the documents and generates correction suggestions
for suspicious words.

- - -
## Table of contents

* [Users](#users-content-users)
* [User management](#user-content-user-management)
* [Projects](#user-content-projects)
    * [Uploading new projects](#user-content-upload-new-project)
	* [Project archives](#user-content-project-archives)
	* [Project table](#user-content-project-table)
	* [Downloading projects](#user-content-download-project)
	* [Profiling projects](#user-content-profile-project)
* [Post correction](#user-content-post-correction)
* [Pocoweb backend](#user-content-pocoweb-backend)

- - -
<a id='users'></a>
## Users

In order to use Pocoweb, each user has to login in the
[login page](/login.php) using her username and password.

There are two kinds of user accounts in Pocoweb, that have
different permissions in the system:

1. Administrator accounts
    * Create new administrator and normal users accounts
    * Delete user accounts
    * Upload new documents and create new [projects](#user-content-project-management)
    * Split projects into [packages](#user-content-project-management)
    * Assign packages to different user accounts
    * Profile projects.
    * Post correct projects and packages.
    * Delete documents and packages.
    * Download corrected projects

2. Normal user accounts
    * Post correct packages that where assigned to them
    * Profile projects.
    * Reassign packages back to their original owner.

- - -
<a id='user-management'></a>
## User management

User management is done via the [Users](/users.php) tab.
You have to be logged in with an administrator account in order
to access and use the user management page.

The user management page consits of a mask to create new users and
a user table that lists all existing users. To create a new user,
fill out the input fields accordingly and then click to the
![create user](img/doc/button-create-user.png) button.
If the account should be an administrator account, do not forget to
check the admin checkbox.

After the mask follows the user table.
In order to remove an user account click on the
![remove](img/doc/glyphicon-remove.png) button in the according user entry
in the user table.
It is not possible at the moment to delete an existing admin account.
Only regular user accounts can be deleted.

- - -
<a id='projects'></a>
## Projects

Project management is done via the [Projects](/index.php) tab.
On this page you can manage the projects and packages (see below) that your
user owns.

Pocoweb uses two different kinds of correctable documents.
*Projects* on the one hand represent whole documents that should be corrected.
*Packages* on the other hand are smaller subsets of projects that contain
a smaller subset of the pages of their parent project.
Two packages with the same parent never contain overlapping pages.
This makes it possible for two different users to correct different packages
of the same project in parallel.

Only administrators can upload new projects, split them into a number of
packages and assign those packages to different users.
It is not possible to assign a project to a different user.

*Note: Since is possible to correct a whole project as well,
one should never correct a project while other users could be correcting
an associated package.
This could lead to cases where one user accidentally
overrides an other users work.*

<a id='upload-new-project'></a>
### Uploading new projects

If you are logged in with an administrator account, you can create a new project.
Fill in the metadata fields in the mask, select the according
[project archive](#user-content-project-archives) on your computer and click
on the ![upload](img/doc/button-upload.png) button.
Depending on the size of the project archive,
the uploading of the project can take a while.

The [Pocoweb backend](#user-content-pocoweb-backend) analyzes the project archive,
processes its OCR and image files and publishes the project.
If the uploading was successfull, you should see the new project in the
project table.

The project's author, title and year can be filled out as it seems fit.
Theses values are not used internally and are there for an easy reference.

A project's profiler and its language are settings specific for the language
profiler.
If you plan to use a profiler for the post correction of your project, you
have to set them accordingly.
If the installation of Pocoweb comes with a local configured profiler,
you can use `local` as value for your profiler.
If not or if you want to use another profiler, you have to specifiy its URL.

Do not forget to set the right language for the new project.
The language field lists all available languages for your chosen profiler.

<a id='project-archives'></a>
### Project archives

A project archive is a zipped directory structure that contains the OCR and
image files for the pages of a document.
Image files should be encoded as PNG files if possible, but JPEG or TIFF
encoded images are fine, too.
The OCR files should be either
[ABBYY-XML](),
[ALTO-XML]() or
[hOCR]()
encoded files.

The backend tries its best to automatically search and order the pages
in project archives.
Therefore the matching OCR and image files should have the same name
(without the extension).
It does not matter in which directory structure the different image and OCR
files reside.
If the matching OCR and image files do have the same filenames for any reason,
you can add a [METS/MOTS]() metadata file to the archive, in which the
page ordering and the association between image and OCR files is specified.

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

There is no need to seperate the image and ocr files with different directories.
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
### Project table

After the create project mask, a table with all the projects
and packages that you own is shown.
If you are logged in with an administrator account you see all your uploaded
projects and all packages that you have not yet assigned to another user.
If you are logged in with a normal user account, you only see the packages
that have been assigned to you.

You can manage the projects by clicking on the according buttons
in the rows of the table:

1. Click on the ![open](img/doc/glyphicon-open-project.png) button to open
the project and start [correcting](#user-content-post-correction) it.

2. Click on the ![download](img/doc/glyphicon-download-project.png) button
to [download](#user-content-download-project) the project.

3. Click on the ![remove](img/doc/glyphicon-remove-project.png) button
to remove the project.

4. Click on the ![order](img/doc/glyphicon-order-profile.png) button to
order a [document profile](#user-content-profile-project).

5. Click on the ![adaptive](img/doc/glyphicon-list-adaptive-tokens.png) button
to see a list of all [adptive tokens](#user-content-profile-project)
found during the profiling.

6. Choose a number of packages and then click on the
![split](img/doc/glyphicon-split-project.png) button
to split the project into multiple packages.

7. Choose an user and click on the
![assign](img/doc/glyphicon-assign-to-user.png) button to assign
a package to an user.

<a id='download-project'></a>
### Downloading projects

You can click on the ![download button](img/doc/glyphicon-download-project.png)
button in the project table to download a project.
The pocoweb backend will then generate a project archive and give
you a link from where you can download it.

The project archive will retain the same structure as the project archive,
that was used to create the project. All corrections are written back into
the original OCR files.

Additionally the archive contains a list of all adaptive tokens of the last
profiler run over the document and a page an line segmented directory structure,
that can directly be used with ocropy.

<a id='profile-project'></a>
### Profiling projects

You can order a document profile by clicking on the
![order profile](img/doc/glyphicon-order-profile.png) button in the project
table. This will start a profiler that profiles the document. The profiler
uses the language settings that where given when the profject was created.

The profiling is done in the background. If the profiling is done,
various helpers are available for the post correction of the profiled project
and its associated packages.

After the profiling is done, you can inspect the list of *adaptive tokens*.
Just click to the
![adaptive token list](img/doc/glyphicon-list-adaptive-tokens.png)
button on the project table.

Adaptive tokens are generated by the profiler.
Whenever a corrected token is found during the profiling,
the profiler tries to match the correction to a known dictionary entry.
If no such match can be generated, a new adaptive token is generated and
inserted into a temporary dictionary for this document. For this reason
Adaptive tokens are possible candidates for new dictionary entries.

For more information about the profiling and adaptive tokens
see the [profiler paper]() and [profiler paper 2]()

- - -
<a id='post-correction'></a>
## Post correction


- - -
<a id='pocoweb-backend'></a>
## Pocoweb backend
