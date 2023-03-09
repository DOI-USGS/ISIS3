import logging
import os
import re as rgx
from requests import get, post
from requests import Response
from requests.exceptions import HTTPError, RequestException
import sys

# Constants
GITHUB_TOKEN=os.environ["GITHUB_TOKEN"]
GITHUB_API_URL=os.environ["GITHUB_API_URL"]
GITHUB_SHA=os.environ["GITHUB_SHA"]

BASE_URL=f'{GITHUB_API_URL}/repos/chkim-usgs/ISIS3'
PULLS_URL=f'{BASE_URL}/pulls'
COMMITS_URL=f'{BASE_URL}/commits/'
ISSUES_URL=f'{BASE_URL}/issues'
HEADERS = {
    "Accept": "application/vnd.github+json",
    "Authorization" : f"Bearer {GITHUB_TOKEN}",
    "X-GitHub-Api-Version": "2022-11-28"
}

def get_prs_associated_with_commit() -> Response:
# Get list of PRs associated with commitSHA
    try:
        # TODO: Remove below pull all PRs [TESTING ONLY]
        # GET_PR_LIST_URL=f'{BASE_URL}/pulls?state=all'
        # response = get(GET_PR_LIST_URL, headers=HEADERS)

        response = get(f'{COMMITS_URL}/{GITHUB_SHA}/pulls', headers=HEADERS)
        response.raise_for_status()
        return response
    except HTTPError as he:
        raise HTTPError("HTTPError in retrieving list of PRs", he) 
    except RequestException as re:
        raise RequestException("Unable to retrieve list of PRs associated with commit.", re)

def get_pr_attributes(response: Response) -> tuple:
    # Get necessary PR attributes
    pull_response_json = response.json()
    if len(pull_response_json) == 0:
        print(False)
        sys.exit(1)
    pull_number = pull_response_json.get("number")
    pull_body = pull_response_json.get("body")
    return (pull_number, pull_body)

def search_for_linked_issues(pull_body: str) -> list:
    """
    Search for linked issues in PR body.
    Regex examples:
    '#1' - matches
    '#123456' - matches
    '# 123' - fails
    '#ABC' - fails
    '## ABC'- fails
    """
    linked_issues = rgx.findall('#[^\D]\d*', pull_body)
    issue_numbers = []
    for linked_issue in linked_issues:
        # Strip linked issue text of '#'
        issue_numbers.append(linked_issue.replace('#',''))
    # print("ISSUE NUMBERS: " + str(issue_numbers))
    return issue_numbers

def get_linked_issues(issue_numbers: list) -> Response:
    # Verify issues exists and has labels
    for issue_number in issue_numbers:
        # Get labels from issue
        try:
            response = get(f'{ISSUES_URL}/{issue_number}', headers=HEADERS)
            response.raise_for_status()
            return response
        except HTTPError as he:
            raise HTTPError("HTTPError in retrieving issues", he)
        except RequestException as re:
            raise RequestException("Unable to retrieve issues.", re)

def get_issue_labels(response: Response) -> list:
    """
    Get labels from all linked issues.
    """
    combined_issue_labels = []
    # Combine labels into a list
    issue_response_json = response.json()
    issue_labels = issue_response_json.get("labels")
    for issue_label in issue_labels:
        # Get name of each label object
        label_name = issue_label.get("name")
        combined_issue_labels.append(label_name)
    # print("COMBINED ISSUE LABELS: " + str(combined_issue_labels))
    return combined_issue_labels

def convert_issue_list_to_dict(combined_issue_labels: list) -> dict:
    # Convert label list into JSON-formatted dict
    labels_data = {}
    labels_data["labels"] = combined_issue_labels
    return labels_data

def update_pr_labels(pull_number: str, labels_data: dict):

    # Update pull request with labels
    # Source: https://stackoverflow.com/q/68459601
    try:
        response = post(f'{ISSUES_URL}/{pull_number}/labels', json=labels_data, headers=HEADERS)
        logging.info("UPDATED RESPONSE: " + str(response.json()))
        response.raise_for_status()
    except HTTPError as he:
        raise HTTPError("HTTPError in updating PR.", he)
    except RequestException as re:
        raise RequestException("Unable to update PR.", re)

def get_pr(pull_number: str) -> Response:
    try:
        response = get(f'{PULLS_URL}/{pull_number}', headers=HEADERS)
        response.raise_for_status()
        return response
    except HTTPError as he:
        raise HTTPError("HTTPError in retrieving issues", he)
    except RequestException as re:
        raise RequestException("Unable to retrieve issues.", re)

def is_pr_bugfix(response: Response):
    labels = response.json().get("labels")
    logging.info("Labels: " + str(labels))
    for label in labels:
        if label.get("name") == "bug":
            logging.info("PR is a bugfix")
            return True
    logging.info("PR is not a bugfix")
    return False

if __name__ == "__main__":
    try:
        response = get_prs_associated_with_commit()
        pull_number, pull_body = get_pr_attributes(response)
        issue_numbers = search_for_linked_issues(pull_body)
        response = get_linked_issues(issue_numbers)
        combined_issue_labels = get_issue_labels(response)
        labels_data = convert_issue_list_to_dict(combined_issue_labels)
        update_pr_labels(pull_number, labels_data)

        # Check PR labels for 'bug'
        response = get_pr(pull_number)
        print(is_pr_bugfix(response))
    except (HTTPError, RequestException, Exception):
        raise

