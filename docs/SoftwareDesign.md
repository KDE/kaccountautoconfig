# Software Design - AutoKonfig Library

## Context

AutoConfig is a protocol (currently being standardized via RFC) that allows a client to automatically configure a mail account, calendar, and other services.

On the KDE PIM side, there is a partial implementation in kmail-account-wizard. The goal is to have a reusable library for KDE to retrieve the information needed to create resources (IMAP, CalDAV, etc.) or accounts for other services.

## References

There are currently two RFC drafts:

| RFC | Format | Link |
|-----|--------|------|
| draft-ietf-mailmaint-autoconfig | XML | https://benbucksch.github.io/autoconfig-spec/draft-ietf-mailmaint-autoconfig.html |
| draft-bucksch-mailmaint-pacc-00 | JSON | https://www.ietf.org/archive/id/draft-bucksch-mailmaint-pacc-00.html#name-servers |

Notable differences:

- **Format**: the first RFC uses XML, the second JSON.
- **Priority**: the first RFC defines that if the same element with the same protocol appears multiple times, the first one takes priority. The second RFC cannot have multiple identical elements with the same protocol.

There is also a third RFC for caldav and carddav: https://datatracker.ietf.org/doc/html/rfc6764#page-5

## Protocol

**How it works:** the protocol extracts the domain from the email address, then queries well-known URLs, ISPDB, MX, or a local file. This allows retrieving the configuration parameters.

## Configuration Format

Here is an example configuration file (XML format):

```xml
<?xml version="1.0"?>
<clientConfig version="1.2">
    <emailProvider id="example.com">
        <domain>example.com</domain>
        <domain>example.net</domain>

        <displayName>Google Workspace</displayName>
        <displayShortName>GMail</displayShortName>

        <incomingServer type="imap">
            <hostname>imap.example.com</hostname>
            <port>993</port>
            <socketType>SSL</socketType>
            <authentication>password-cleartext</authentication>
            <username>%EMAILADDRESS%</username>
        </incomingServer>
        <incomingServer type="pop3">
            <hostname>pop.example.com</hostname>
            <port>995</port>
            <socketType>SSL</socketType>
            <authentication>password-cleartext</authentication>
            <username>%EMAILADDRESS%</username>
        </incomingServer>

        <outgoingServer type="smtp">
            <hostname>smtp.googlemail.com</hostname>
            <port>587</port>
            <socketType>STARTTLS</socketType>
            <authentication>password-cleartext</authentication>
            <username>%EMAILADDRESS%</username>
        </outgoingServer>

        <documentation url="https://www.example.com/help/mail/">
            <descr lang="en">Configure mail app for IMAP</descr>
            <descr lang="de">Email mit IMAP konfigurieren</descr>
        </documentation>
    </emailProvider>

    <calendar type="caldav">
        <url>https://calendar.example.com/remote.php/dav</url>
        <authentication>basic</authentication>
        <username>%EMAILADDRESS%</username>
    </calendar>

    <addressbook type="carddav">
        <url>https://contacts.example.com/remote.php/dav</url>
        <authentication>basic</authentication>
        <username>%EMAILADDRESS%</username>
    </addressbook>

</clientConfig>
```

Here is an example configuration file (JSON format):

```
{
  servers: {
    jmap: {
      url: "https://jmap.example.net/session",
      authentication: [
        "OAuth2",
        "http-basic",
      ]
    },
    imap: {
      url: "imaps://imap.example.net",
      authentication: [
        "OAuth2",
        "sasl-plain",
      ]
    },
    pop3: {
      url: "imaps://pop3.example.net",
      authentication: [
        "OAuth2",
        "sasl-SCRAM-SHA-256-PLUS",
        "sasl-plain",
        "native"
      ]
    },
    smtp: {
      url: "imaps://pop3.example.net",
      authentication: [
        "OAuth2",
        "sasl-SCRAM-SHA-256-PLUS",
        "sasl-plain"
      ]
    },
    caldav: {
      url: "https://sync.example.net/calendar/",
      authentication: [
        "OAuth2",
        "http-basic",
      ]
    },
    carddav: {
      url: "https://sync.example.net/contacts/",
      authentication: [
        "OAuth2",
        "http-basic",
      ]
    }
  },
  oAuth2: {
    issuer: "auth.example.net"
  },
  provider: {
    name: "ACME BestService WorkPlaceMail",
    shortName: "ACME",
    logo: [
      {
        url: "https://www.example.net/logo.svg",
        mimetype: "image/svg"
      },
      {
        url: "https://www.example.net/logo.png",
        mimetype: "image/png",
        width: "128",
        height: "128",
      },
      {
        url: "https://www.example.net/logo-512.png",
        mimetype: "image/png",
        width: "512",
        height: "512",
      }
    ]
  },
  help: {
    documentation: "https://help.example.net/howto/set-up-your-mail-app.html",
    developer: "https://developer.example.net/client-apps/",
    contact: "mailto:it@team.example.net"
  },
  versi
```

## Functional Requirements

- **GUI-less** library
- Must integrate into account-wizard and KCM
- Provides the data needed to create resources
- Can return multiple elements from a single mail/password pair

The library is limited to:

- fetching the configuration file
- returning a list of elements (see terminology above)

It is up to the library's client to choose and use the elements it needs. The library DOES NOT CREATE ACCOUNTS OR RESOURCES (in the KDE sense) — it is up to the library's client to create the resources.

### Terminology

The terminology used is that of the RFC. The following element exist:

| Element | Description |
|---------|-------------|
| `incomingServer` | Incoming server (IMAP, POP3) |
| `outgoingServer` | Outgoing server (SMTP) |
| `calendar` | Calendar (CalDAV) |
| `addressbook` | Address book (CardDAV) |

Each element contains the information needed to configure a resource.

From now on, the term **protocol** refers to one of the following values:

- IMAP
- POP3
- SMTP
- CalDAV
- CardDAV

Some others values may be added

## PUBLIC API Design

Basically the public api is a KJob that will return a list of elements. One element is returned per protocol and will contain the information needed to configure a resource or an account.

PSEUDO CODE

```
Class AutoConfigJob: KJob
  AutoConfigJob(email, password)
  
  // Returns an element by protocol, meaning only one element per protocol is available.
  GetElement(Protocol protocol)
  // Returns all elements
  GetElements()
  
  Signal:
    Progress(int progress, Element element)
    Error(Enum error)
    
Class Element:
  Enum Protocol           // IMAP, POP3, etc.
  URL url
  Enum authenticationType // basic, etc
  bool   isValid()       // Indicates that the login succeeded (implying everything works) with the authenticationType, aka status == LOGIN_SUCCESS
  Enum   Status        //   
```

The library will return one element per protocol, because the second RFC does not allow having the same element with the same protocol multiple times.

### JOB Error Handling

```
Enum JobError:
  // data not found across all resolution mechanisms
  DATA_NOT_FOUND
  // Invalid configuration file format for both formats
  INVALID_DATA
  // probably many others
```

Retrieving elements is possible if and only if there is no error at the JOB level.

### Status Handling

If the job does not error, the job can return the elements. An element has a status that the client must take into account.

```
Enum Status:
  // The login test succeeded, so the configuration is correct
  LOGIN_SUCCESS
  // the credentials are incorrect
  BAD_CREDENTIALS
  // probably many others
```

Note: with the same mail/password pair, one element may have the status LOGIN_SUCCESS while another has BAD_CREDENTIALS. For example, the IMAP and SMTP elements with LOGIN_SUCCESS, and the CalDAV element with BAD_CREDENTIALS. It is up to the client to determine whether this is a problem.

### Configuration File Resolution Strategy

There is a well-defined call order defined by the RFC. For performance reasons, the library will make as many calls in parallel as possible.

For example, it will call in parallel: 1)

- https://autoconfig.%EMAILDOMAIN%/mail/config-v1.1.xml?emailaddress=%EMAILADDRESS%
- https://%EMAILDOMAIN%/.well-known/autoconfig/mail/config-v1.1.xml

Then if unsuccessful: 2)

- https://v1.ispdb.net/%EMAILDOMAIN%

Then if unsuccessful:

- MX followed by 1

### Cache Override

For testing purposes, and potentially for a pre-configured machine scenario, an override is possible by providing a local file on disk (following the RFC format) in JSON or XML.

### User Validation

The RFC clearly states that the user must confirm the configuration and only then perform the login test. To avoid back-and-forth, the Job will validate the login before the user confirms.

## Ticket Breakdown

- Bootstrap
- Implement PUBLIC API without resolution
- Parse XML
- Implement well-known call
- Implement ISPDB call
- Implement MX call
- Implement local file call
- Implement resolution strategy, aka chaining the calls
- Implement the login test
- Glue the resolution strategy and the login test to the public API
- E2E Tests

**Note that, at first, only the AutoConfig RFC will be implemented. The PACC specification is useful for the design, but it is not needed for now.**