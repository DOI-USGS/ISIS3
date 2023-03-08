import requests
import json
import re
import os


def main():
    # Constants
    GITHUB_TOKEN=os.environ["GITHUB_TOKEN"]
    GITHUB_API_URL=os.environ["GITHUB_API_URL"]
    GITHUB_SHA=os.environ["GITHUB_SHA"]

    BASE_URL=f'{GITHUB_API_URL}/repos/chkim-usgs/ISIS3'
    COMMITS_URL=f'{BASE_URL}/commits/'
    ISSUES_URL=f'{BASE_URL}/issues'
    HEADERS = {
        "Accept": "application/vnd.github+json",
        "Authorization" : f"Bearer {GITHUB_TOKEN}",
        "X-GitHub-Api-Version": "2022-11-28"
    }

    # Get list of PRs associated with commitSHA
    try:
        # TODO: Remove below pull all PRs [TESTING ONLY]
        GET_PR_LIST_URL=f'{BASE_URL}/pulls?state=all'
        response = requests.get(GET_PR_LIST_URL, headers=headers)

        # response = requests.get(f'{COMMITS_URL}/{GITHUB_SHA}/pulls', headers=HEADERS, verify='/Users/chkim/homebrew/etc/ca-certificates/cert.pem')
        response.raise_for_status()
    except requests.exceptions.HTTPError as he:
        print("HTTPError in retrieving list of PRs", he)
        sys.exit(1)
        raise 
    except requests.exceptions.RequestException as re:
        print("Unable to retrieve list of PRs associated with commit.", re)
        sys.exit(1)

    # Get necessary PR attributes
    pull_response_json = response.json()
    pull_number = pull_response_json[0].get("number")
    pull_body = pull_response_json[0].get("body")

    """
    Search for linked issues in PR body.
    Regex examples:
    '#1' - matches
    '#123456' - matches
    '# 123' - fails
    '#ABC' - fails
    '## ABC'- fails
    """
    linked_issues = re.findall('#[^\D]\d*', pull_body)
    issue_numbers = []
    for linked_issue in linked_issues:
        # Strip linked issue text of '#'
        issue_numbers.append(linked_issue.replace('#',''))

    # Verify issues exists and has labels
    combined_issue_labels = []
    for issue_number in issue_numbers:

        # Get labels from issue
        try:
            response = requests.get(f'{ISSUES_URL}/{issue_number}', headers=HEADERS)
            response.raise_for_status()
        except requests.exceptions.HTTPError as he:
            print("HTTPError in retrieving list of PRs", he)
            sys.exit(1)
            raise 
        except requests.exceptions.RequestException as re:
            print("Unable to retrieve list of PRs associated with commit.", re)
            sys.exit(1)
        
        # Combine labels into a list
        issue_response_json = response.json()
        issue_labels = issue_response_json.get("labels")
        for issue_label in issue_labels:
            # Get name of each label object
            label_name = issue_label.get("name")
            combined_issue_labels.append(label_name)
    print(combined_issue_labels)

    # Convert label list into JSON-formatted dict
    labels_data = {}
    labels_data["labels"] = combined_issue_labels

    # Update pull request with labels
    # Source: https://stackoverflow.com/q/68459601
    try:
        response = requests.post(f'{ISSUES_URL}/{pull_number}/labels', json=labels_data, headers=HEADERS)
        response.raise_for_status()
    except requests.exceptions.HTTPError as he:
        print("HTTPError in retrieving list of PRs", he)
        sys.exit(1) 
    except requests.exceptions.RequestException as re:
        print("Unable to retrieve list of PRs associated with commit.", re)
        sys.exit(1)

    sys.exit(0)


if __name__ == "__main__":
    main()