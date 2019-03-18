import string

from django.core.exceptions import ValidationError


class UsernameValidator:
    _min_length = 4
    _allowed_punctuation = '_'

    def __call__(self, username):
        allowed_chars = ''.join([string.ascii_letters, string.digits, UsernameValidator._allowed_punctuation])

        if len(username) < UsernameValidator._min_length:
            raise ValidationError('Username too short; at least {} characters required'.format(UsernameValidator._min_length))
        for c in username:
            if c not in allowed_chars:
                raise ValidationError('Incorrect username; \'{}\' character not allowed'.format(c))

    def get_help_text(self):
        return '<ul><li>Username must contain at least {} characters;<br/>lowercase-letters, uppercase-letters, digits and \'{}\' are allowed</li></ul>'.format(
            UsernameValidator._min_length,
            UsernameValidator._allowed_punctuation
        )

