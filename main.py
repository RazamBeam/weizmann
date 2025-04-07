import pygame
import subprocess

a = []
turn = 1

def run(path, args):
    r = subprocess.run([path] + args, capture_output=True, text=True)
    print(r.stdout)
    return r.returncode


screen_size = (800, 800)
size = screen_size[0]/8

white = (255, 255, 255)
gray = (155, 155, 155)
light_gray = (200, 200, 200)
black = (0, 0, 0)
yellow = (220, 220, 0)

running = True
screen = pygame.display.set_mode(screen_size)


whites = 0
blacks = 0
kings = 0


def getIndex(i, j):
    return (7-i)*4 + ((7-j) >> 1)


def quit_game():
    if event.type == pygame.QUIT:
        quit()


def draw_screen():
    screen.fill(white)

    for i in range(8):
        for j in range(8):
            if (i+j) % 2 == 0:
                pygame.draw.rect(screen, (230, 190, 130), (j * size, i*size, size, size))
                continue
            else:
                pygame.draw.rect(screen, (177, 110, 65), (j * size, i * size, size, size))

            p = 1 << getIndex(i, j)

            if p & whites:
                pygame.draw.circle(screen, white, ((j+0.5)*size, (i+0.5)*size), size * 0.3)
                if p & kings:
                    pygame.draw.circle(screen, black, ((j + 0.5) * size, (i + 0.5) * size), size * 0.2, round(size * 0.05))
            if p & blacks:
                pygame.draw.circle(screen, black, ((j+0.5)*size, (i+0.5)*size), size * 0.3)
                if p & kings:
                    pygame.draw.circle(screen, white, ((j + 0.5) * size, (i + 0.5) * size), size * 0.2, round(size * 0.05))

    pygame.display.update()


while running:

    events = pygame.event.get()

    p = 1
    arg = ""
    for i in range(32):
        if p & whites:
            if p & kings:
                arg += "W"
            else:
                arg += "w"
        elif p & blacks:
            if p & kings:
                arg += "B"
            else:
                arg += "b"
        else:
            arg += "."
        p <<= 1
    #print(arg)

    if len(events) > 0:
        for event in events:
            quit_game()

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_a:
                    a.append(whites)
                    print(a)
                if event.key == pygame.K_SPACE:
                    print(arg)
                    move = run("main.exe", ["w" if (turn%2==0) else "b", arg])
                    if turn == 0:
                        a = move & blacks
                        whites ^= move & (~a)
                        blacks ^= a
                    else:
                        a = move & whites
                        blacks ^= move & (~a)
                        whites ^= a
                    turn = not turn

            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_x, mouse_y = event.pos
                j = mouse_x // size
                i = mouse_y // size

                if (i+j) % 2 == 1:
                    p = 1 << getIndex(int(i), int(j))

                    if event.button == 1:
                        whites ^= p
                        blacks &= ~p
                    elif event.button == 3:
                        blacks ^= p
                        whites &= ~p
                    elif event.button == 2:
                        kings ^= p
                    kings &= whites | blacks
                print(whites)

    draw_screen()