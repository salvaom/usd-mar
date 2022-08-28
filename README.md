> **Warning**
> This is probably my first CMake/C++ project, you might see some things that could be better or are simply wrong, so
> please take it with a pinch of salt. PRs are highly appreciated.

# USD Multi Asset Resolver

An ArResolver plugin for USD. This project aims to provide a wide range of tools to make the asset resolution on USD
much more flexible. The resolver can be configured with stacks of sub-solvers that will perform tasks on the asset path
such as expanding environment variables or performing a REST query to retrieve asset paths from a database (and in the
future, possibly more).

# Usage

The asset resolver can be configured with stacks of sub-solvers that will pick up the output of the previous sub-solver
and pass its result to the next one until the asset path is fully resolved.

## Available subsolvers

| Name    | Purpose                                | Notes       |
|---------|----------------------------------------|-------------|
| env     | Expands environment variables          | Implemented |
| rest    | Makes a GET request to a URL           | Implemented |
| format  | Parses and re-formats                  | Implemented |
| symlink | Expands symlinks                       | Planned     |
| cache   | Caches values to be baked for the farm | Planned     |

## Configuration

The solver is configured by specifying the path to a JSON file via the environment variable `USDMAR_CONFIG_PATH` with
the following format:

```json
{
    "stacks": {
        "<stack name>": {
            "resolvers": [
                {
                    "type": "<subsolver type>",
                    "<subsolver var>": "<value>"
                }
            ]
        }
    },
    "defaults": {
        "<scheme>": "<stack>"
    }
}
```

Under `stacks`, a mapping of stacks can be defined, each one with its own list of resolvers. Resolvers can also specify
their own variables. The defaults key makes defining the default stack to use in the asset path optional, for more
information see the next section.

## Schemes

The asset path is composed of three elements: `<scheme>:<stack>!<path>`, where `scheme` is the defined scheme at build
time (by default `mr`), `stack` is the name of the stack to use and `path` is the asset path itself to be resolved. On
the next section there are examples on it's usage.

## Example

The following configuration file will do the following:

* Convert a path in the format of `/{project}/seq/{sequence}/{shot}/{asset}/{version}@` to a valid URL
* Make a GET request to a local REST server
* Expand any environment variables on the path

```json
{
    "resolvers": {
        "db": {
            "stack": [
                {
                    "type": "format",
                    "pairs": [
                        [
                            "/([\\w\\d]+)/(?:seq|build)/([\\w\\d_]+)/([\\w\\d_]+)/([\\w\\d_]+)/([\\w\\d_]+)",
                            "?project={0}&sequence={1}&shot={2}&asset={3}&version={4}"
                        ]
                    ]
                },
                {
                    "type": "rest",
                    "host": "http://127.0.0.1:5000",
                    "entryPoint": "/api/v3"
                },
                {
                    "type": "env"
                }
            ]
        }
    },
    "defaults": {
        "mr": "db"
    }
}
```

To test this, we can create this USD file and run a simple Flask server on localhost with port 5000. Please note that
because we've defined in the `defaults` section the scheme `mr:`, the asset path specified would be equivalent to it
without the `db!` specification: `@mr:/AHB/seq/NJS/0010/boat_aa/latest@`.

```
#usda 1.0
(
)

def "geo" (
	references = [
        @mr:db!/AHB/seq/NJS/0010/boat_aa/latest@
    ]
    ) {
}
```

We can set the environment variable `TF_DEBUG` to  `USDMAR"` to get a better feeling of what's going on:

```
usdmar - MultiResolver::_Resolve('mr:db!/AHB/seq/NJS/0010/boat_aa/latest')
usdmar - Extracted asset path: /AHB/seq/NJS/0010/boat_aa/latest
usdmar - [RESTSubSolver::Resolve] Making GET request to url 'http://127.0.0.1:5000/api/v3?project=AHB&sequence=NJS&shot=0010&asset=boat_aa&version=latest'
usdmar - [RESTSubSolver::Resolve] Response: ${PROJECTS_ROOT}/AHB/publish/seq/NJS/0010/boat_aa/v005/main.usda
usdmar - Resolving /AHB/seq/NJS/0010/boat_aa/latest:
        format    -> ?project=AHB&sequence=NJS&shot=0010&asset=boat_aa&version=latest
        rest      -> ${PROJECTS_ROOT}/AHB/publish/seq/NJS/0010/boat_aa/v005/main.usda
        env       -> /mnt/projects/AHB/publish/seq/NJS/0010/boat_aa/v005/main.usda
```

# Building

## Build command

The easiest way to build it is to compile USD and use the flag `FETCH_CPR` to automatically download and build the
missing of dependency:

```
git clone https://github.com/salvaom/usd-mar
cd usd-mar
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=<USD INSTALL PATH> -G "<GENERATOR NAME>" -DFETCH_CPR=ON ..
```

## Build requirements:

| Name                                 | Optional    | Version |
|--------------------------------------|-------------|---------|
| USD                                  | No          | 22.08   |
| [CPR](https://github.com/libcpr/cpr) | Yes         | 1.9.1   |
| Python                               | No, planned | 3.7.x   |

## Build Options

| Option           | Function                                                              | Default  |
|------------------|-----------------------------------------------------------------------|----------|
| USDMAR_REST      | Whether to build with support for the REST subsolver<br/>Requires CRP | ON       |
| FETCH_CPR        | Downloads and builds CPR                                              | OFF      |
| RESOLVER_SCHEMES | List of schemes (space separated) to register for asset resolution    | `mr`     |
