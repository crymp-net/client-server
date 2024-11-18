#!/usr/bin/env python3

import argparse
import github
import sys

GITHUB_REPOSITORY = 'crymp-net/client-server'

def parse_command_line() -> argparse.Namespace:
	parser = argparse.ArgumentParser(description='Changelog generator')
	parser.add_argument('--token', required=True, help='GitHub API Token')
	return parser.parse_args()

def main() -> int:
	args = parse_command_line()
	gh = github.Github(auth=github.Auth.Token(args.token))
	repo = gh.get_repo(GITHUB_REPOSITORY)
	main_branch = repo.default_branch
	latest_release_datetime = repo.get_latest_release().created_at
	print('### Resolved issues')
	for issue in repo.get_issues(state='closed'):
		if issue.state_reason == 'completed' and issue.closed_at > latest_release_datetime:
			user = f'[{issue.user.login}]({issue.user.html_url})'
			print(f'- [#{issue.number}]({issue.html_url}): {issue.title} ({user})')
	print()
	print('### Changes')
	for pull in repo.get_pulls(state='closed', base=main_branch):
		if pull.merged_at is not None and pull.merged_at > latest_release_datetime:
			user = f'[{pull.user.login}]({pull.user.html_url})'
			print(f'- [#{pull.number}]({pull.html_url}): {pull.title} ({user})')
	print()
	return 0

if __name__ == '__main__':
	sys.exit(main())
