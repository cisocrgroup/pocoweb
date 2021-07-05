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
## Table of contents
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
  * [GET rest/postcorrect/model/books/`pid`](#user-content-api-get-pcmodel)
  * [POST rest/postcorrect/model/books/`pid`](#user-content-api-post-pcmodel)
  * [GET rest/pool/global](#user-content-api-get-global-pool)
  * [GET rest/pool/user](#user-content-api-get-user-pool)
  * [GET rest/jobs/`jobid`](#user-content-api-get-jobs)

- - -

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
<a id='api-get-pcmodel'></a>
### GET rest/postcorrect/model/books/`pid`
Get the post-correction models available for the given
book `pid`.
* [Authorization](#user-content-authorization) is required.

#### Response data
```json
{
	"projectId": 13,
	"bookId": 11,
	"models": [
		{
			"id": 1,
			"name": "19th",
			"description": "model trained on 19th century documents"
		},
		"..."
	]
}
```

---
<a id='api-post-pcmodel'></a>
### POST rest/postcorrect/model/books/`pid`
Set the post-correction model used for the given book `pid`.
* [Authorization](#user-content-authorization) is required.

#### Post data
```json
{
	"modelId": 1
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
